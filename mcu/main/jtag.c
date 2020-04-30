/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#include "arm.h"
#include "jtag.h"
#include "jtag_lowlevel.h"

#include <stdio.h>
#include <string.h>

#define MAX_JTAG_DEVICES 32

///////////////////////////////////////////////////////////////////////////////

uint64_t sampleCores;

bool dumpPins = false;

static uint8_t recTdi[2048];
static uint8_t recTms[2048];
static uint8_t recRead[2048];
static unsigned seqStart;
static unsigned seqSize;

static uint8_t recReadFlag[64];
static uint16_t recInitPos[64];
static uint16_t recLoopPos[64];
static uint16_t recAckPos[64];
static uint16_t recEndPos[64];
static unsigned progSize;

static uint8_t *tdiPtr;
static uint8_t *tmsPtr;
static uint8_t *readPtr;

static uint8_t *readFlagPtr;
static uint16_t *initPosPtr;
static uint16_t *loopPosPtr;
static uint16_t *ackPosPtr;
static uint16_t *endPosPtr;

static int numDevices = 0;
static struct JtagDevice devices[MAX_JTAG_DEVICES];
uint32_t dpIdcode = 0;

///////////////////////////////////////////////////////////////////////////////

static void startRec(void) {
  seqStart = 0;
  seqSize = 0;
  progSize = 0;
  tdiPtr = recTdi;
  tmsPtr = recTms;
  readPtr = recRead;
  readFlagPtr = recReadFlag;
  initPosPtr = recInitPos;
  loopPosPtr = recLoopPos;
  ackPosPtr = recAckPos;
  endPosPtr = recEndPos;
  *tdiPtr = 0;
  *tmsPtr = 0;
  *readPtr = 0;
}

static void sendRec(void) {
  readWriteSeq(seqSize, recTdi, recTms, NULL);
}

static void readWriteRec(uint8_t *response_buf) {
  readWriteSeq(seqSize, recTdi, recTms, response_buf);
}

static void storeRec(void) {
  unsigned words = seqSize / 8;
  if(seqSize % 8) words++;
  storeSeq(words, recTdi, recTms);
  storeProg(progSize, recReadFlag, recInitPos, recLoopPos, recAckPos, recEndPos);
}

static void recordSeq(unsigned size, uint8_t *tdiData, uint8_t *tmsData, uint8_t *readData) {
  for(int i = 0; i < size; i++) {
    unsigned sourceBit = i % 8;
    unsigned targetBit = seqSize % 8;

    if(*tdiData & (1 << sourceBit)) *tdiPtr |= (1 << targetBit);
    if(*tmsData & (1 << sourceBit)) *tmsPtr |= (1 << targetBit);
    if(readData) {
      if(*readData & (1 << sourceBit)) *readPtr |= (1 << targetBit);
    }

    if(sourceBit == 7) {
      tdiData++;
      tmsData++;
      if(readData) readData++;
    }
    if(targetBit == 7) {
      tdiPtr++;
      tmsPtr++;
      readPtr++;
      *tdiPtr = 0;
      *tmsPtr = 0;
      *readPtr = 0;
    }

    seqSize++;
  }
}

static void recordCommand(uint8_t read, uint16_t initPos, uint16_t loopPos, uint16_t ackPos, uint16_t endPos) {
  *readFlagPtr++ = read;
  *initPosPtr++ = initPos;
  *loopPosPtr++ = loopPos;
  *ackPosPtr++ = ackPos;
  *endPosPtr++ = endPos;
  progSize++;
}

///////////////////////////////////////////////////////////////////////////////

static void fillByte() {
  unsigned bitsToAdd = 0;
  if(seqSize % 8) bitsToAdd = 8 - (seqSize % 8);
  if(bitsToAdd) {
    uint8_t tdi = 0;
    uint8_t tms = 0;
    uint8_t read = 0;
    recordSeq(bitsToAdd, &tdi, &tms, &read);
  }
}

static void gotoResetThenIdle(void) {
  uint8_t tms = 0x1f;
  uint8_t tdi = 0;

  readWriteSeq(6, &tdi, &tms, NULL);
}

static inline void gotoShiftIr(void) {
  uint8_t tms = 0x03;
  uint8_t tdi = 0;

  recordSeq(4, &tdi, &tms, NULL);
}

static inline void exitIrToShiftDr(void) {
  uint8_t tms = 0x03;
  uint8_t tdi = 0;

  recordSeq(4, &tdi, &tms, NULL);
}

static inline unsigned gotoShiftDr(void) {
  uint8_t tms = 0x01;
  uint8_t tdi = 0;

  recordSeq(3, &tdi, &tms, NULL);

  return 3;
}

static inline void exitIrToIdle(void) {
  uint8_t tms = 0x01;
  uint8_t tdi = 0;

  recordSeq(2, &tdi, &tms, NULL);
}

static inline unsigned exitDrToIdle(void) {
  uint8_t tms = 0x01;
  uint8_t tdi = 0;

  recordSeq(2, &tdi, &tms, NULL);

  return 2;
}

static uint32_t lastIr = -1;

static void writeIrInt(uint32_t idcode, uint32_t ir) {
  if(ir != lastIr) {
    lastIr = ir;
    gotoShiftIr();
    for(int i = 0; i < numDevices; i++) {
      struct JtagDevice *dev = &devices[i];
      if(dev->idcode == idcode) {
        uint8_t tms = 0;
        uint8_t tdi = ir;

        if(i == (numDevices - 1)) {
          tms = 1 << (dev->irlen - 1);
        }

        recordSeq(dev->irlen, &tdi, &tms, NULL);

      } else {
        uint8_t tms[4] = {0, 0, 0, 0};
        uint8_t tdi[4] = {~0, ~0, ~0, ~0};

        if(i == (numDevices - 1)) {
          // set tms end bit
          int byte = (dev->irlen-1) / 8;
          int bit = (dev->irlen-1) % 8;
          tms[byte] = 1 << bit;
        }

        recordSeq(dev->irlen, tdi, tms, NULL);
      }
    }
    exitIrToIdle();
  }
}

static bool readWriteDrPre(uint32_t idcode, unsigned *postscan) {
  int total = 0;

  int devNum;
  for(devNum = 0; devNum < numDevices; devNum++) {
    struct JtagDevice *dev = &devices[devNum];
    if(dev->idcode == idcode) {
      break;
    }
  }
  if(devNum == numDevices) {
    printf("Can't find device %x", (unsigned)idcode);
    return false;
  }

  total = gotoShiftDr();
  
  if(postscan) {
    *postscan = numDevices - devNum - 1;
  }

  if(devNum) {
    uint8_t tms[MAX_JTAG_DEVICES/8];
    uint8_t tdi[MAX_JTAG_DEVICES/8];

    memset(tms, 0, MAX_JTAG_DEVICES/8);
    memset(tdi, ~0, MAX_JTAG_DEVICES/8);

    recordSeq(devNum, tdi, tms, NULL);
    total += devNum;
  }

  return true;
}

static unsigned readWriteDrPost(unsigned postscan) {
  // postscan
  if(postscan) {
    uint8_t tms[MAX_JTAG_DEVICES/8];
    uint8_t tdi[MAX_JTAG_DEVICES/8];

    memset(tms, 0, MAX_JTAG_DEVICES/8);
    memset(tdi, ~0, MAX_JTAG_DEVICES/8);

    int byte = (postscan-1) / 8;
    int bit = (postscan-1) % 8;
    tms[byte] = 1 << bit;

    recordSeq(postscan, tdi, tms, NULL);
  }

  return postscan + exitDrToIdle();
}

///////////////////////////////////////////////////////////////////////////////

void jtagWriteIr(uint32_t idcode, uint32_t ir) {
  startRec();
  writeIrInt(idcode, ir);
  sendRec();
}

bool jtagReadWriteDr(uint32_t idcode, uint8_t *din, uint8_t *dout, int size) {
  startRec();
  unsigned postscan;
  if(!readWriteDrPre(idcode, &postscan)) return false;
  sendRec();

  startRec();

  uint8_t tms[256];
  assert(size <= 256);
  memset(tms, 0, size);

  if(!postscan) {
    int byte = (size-1) / 8;
    int bit = (size-1) % 8;
    tms[byte] = 1 << bit;
  }

  recordSeq(size, din, tms, NULL);
  if(dout) readWriteRec(dout);
  else sendRec();
  
  startRec();
  readWriteDrPost(postscan);
  sendRec();

  return true;
}

bool jtagDpLowAccess(uint8_t RnW, uint16_t addr, uint32_t writeValue, uint32_t *readValue) {
  bool apndp = addr & APNDP;
  addr &= 0xff;
  uint8_t ack = ARM_ACK_WAIT;

  uint8_t response_buf[8];
  int tries = 1024;

  startRec();
  writeIrInt(dpIdcode, apndp ? ARM_IR_APACC : ARM_IR_DPACC);

  fillByte();

  unsigned ackPos;
  unsigned dataPos;

  while(tries > 0 && ack == ARM_ACK_WAIT) {
    // create sequence

    unsigned postscan;
    readWriteDrPre(dpIdcode, &postscan);

    { // addr + readbit
      uint8_t tdi = ((addr >> 1) & 0x06) | (RnW?1:0);
      uint8_t tms = 0;
      uint8_t read = 0x07;
      ackPos = seqSize;
      recordSeq(3, &tdi, &tms, &read);
    }

    { // value
      uint8_t tms[4];
      uint8_t read[4];
      memset(tms, 0, 4);
      if(RnW) memset(read, 0xff, 4);
      else memset(read, 0, 4);

      if(!postscan) {
        tms[3] = 0x80;
      }

      dataPos = seqSize;
      recordSeq(32, (uint8_t*)&writeValue, tms, read);
    }

    readWriteDrPost(postscan);

    fillByte();

    readWriteRec(response_buf);

    ack = extractAck(&ackPos, response_buf);

    tries--;
    startRec();
  }

  if(ack == ARM_ACK_WAIT) {
    printf("Error: Invalid ACK (ack == ARM_ACK_WAIT)\n");
    return false;

  } else if(ack != ARM_ACK_OK) {
    printf("Error: Invalid ACK (%x != ARM_ACK_OK)\n", ack);
    return false;
  }

  if(readValue) *readValue = extractWord(&dataPos, response_buf);

  return true;
}

int getNumDevices(void) {
  uint8_t tdi[MAX_JTAG_DEVICES/8];
  uint8_t tms[MAX_JTAG_DEVICES/8];

  startRec();

  // set all devices in bypass by sending lots of 1s into the IR
  gotoShiftIr();
  memset(tdi, 0xff, MAX_JTAG_DEVICES/8);
  memset(tms, 0, MAX_JTAG_DEVICES/8);
  tms[(MAX_JTAG_DEVICES/8)-1] = 0x80;
  recordSeq(MAX_JTAG_DEVICES, tdi, tms, NULL);

  // flush DR with 0
  exitIrToShiftDr();
  memset(tdi, 0, MAX_JTAG_DEVICES/8);
  memset(tms, 0, MAX_JTAG_DEVICES/8);
  recordSeq(MAX_JTAG_DEVICES, tdi, tms, NULL);

  sendRec();

  // find numDevices by sending 1s until we get one back
  int num;

  for(num = 0; num < MAX_JTAG_DEVICES; num++) {
    uint8_t tms = 0;
    uint8_t tdi = 0x01;
    uint8_t tdo;

    readWriteSeq(1, &tdi, &tms, &tdo);
    if(tdo & 1) break;
  }

  gotoResetThenIdle();

  return num;
}

static unsigned getIrLen(uint32_t idcode, struct JtagDevice *devlist) {

  for(int i = 0; i < MAX_JTAG_DEVICES; i++) {
    if(idcode == devlist[i].idcode) {
      return devlist[i].irlen;
    }
  }

  printf("Unknown IDCODE %x\n", (unsigned)idcode);

  return 0;
}

static void getIdCodes(uint32_t *idcodes) {
  startRec();
  gotoShiftDr();
  sendRec();

  for(int i = 0; i < numDevices; i++) {
    uint8_t tms[4] = {0, 0, 0, 0};
    uint8_t tdi[4] = {0, 0, 0, 0};
    uint8_t tdo[4];

    readWriteSeq(32, tdi, tms, tdo);

    unsigned pos = 0;

    *idcodes++ = extractWord(&pos, tdo);
  }

  gotoResetThenIdle();
}

static bool jtagQueryChain(struct JtagDevice *devlist) {
  numDevices = getNumDevices();

  if((numDevices == 0) || (numDevices == MAX_JTAG_DEVICES)) {
    addLogLine("No JTAG chain found\n");
    return false;

  } else {
    uint32_t idcodes[MAX_JTAG_DEVICES];
    getIdCodes(idcodes);
    for(int i = 0; i < numDevices; i++) {
      if(idcodes[i]) {
        struct JtagDevice dev;
        dev.idcode = idcodes[i];
        dev.irlen = getIrLen(dev.idcode, devlist);
        if(!dev.irlen) return false;
        devices[i] = dev;
        addLogLine("Device %d: idcode %x\n", i, (int)devices[i].idcode);
      } else {
        numDevices = i;
        break;
      }
    }
  }

  return true;
}

#if 0

void coreReadPcsrInit(uint64_t coresToSample) {
  sampleCores = coresToSample;
}

bool coreReadPcsrFast(uint64_t *pcs, bool *halted) {
  unsigned core = 0;

  for(unsigned i = 0; (i < numCores) && (i < MAX_CORES); i++) {
    if(sampleCores & (1 << i)) {
      if(cores[i].enabled) {
        pcs[core] = coreReadPcsr(&cores[i]);
      } else {
        pcs[core] = 0;
      }
      core++;
    }
  }

  if(cores[0].type == ARMV8A) {
    *halted = coreHalted(firstCore);
  } else {
    *halted = pcs[0] == 0xffffffff;
  }

  return true;
}

#else

static unsigned fastBitsToRead;

static void dpLowAccessFast(uint8_t RnW, uint16_t addr, uint32_t writeValue, bool discard) {
  bool apndp = addr & APNDP;
  addr &= 0xff;

  uint16_t initPos = seqSize;

  writeIrInt(dpIdcode, apndp ? ARM_IR_APACC : ARM_IR_DPACC);

  fillByte();

  uint16_t loopPos = seqSize;

  unsigned postscan;
  readWriteDrPre(dpIdcode, &postscan);

  uint16_t ackPos = seqSize;

  { // addr + readbit
    uint8_t tdi = ((addr >> 1) & 0x06) | (RnW?1:0);
    uint8_t tms = 0;
    recordSeq(3, &tdi, &tms, NULL);
    fastBitsToRead += 3;
  }

  { // value
    uint8_t tms[4];
    memset(tms, 0, 4);

    if(!postscan) {
      tms[3] = 0x80;
    }
    recordSeq(32, (uint8_t*)&writeValue, tms, NULL);

    if(!discard) fastBitsToRead += 32;
  }

  readWriteDrPost(postscan);

  fillByte();

  uint16_t endPos = seqSize;

  recordCommand(!discard, initPos, loopPos, ackPos, endPos);
}

static void dpReadFast(uint16_t addr) {
  dpLowAccessFast(LOW_READ, addr, 0, true);
  dpLowAccessFast(LOW_READ, DP_RDBUFF, 0, false);
}

static void dpWriteFast(uint16_t addr, uint32_t value) {
  dpLowAccessFast(LOW_WRITE, addr, value, true);
}

static void apWriteFast(unsigned apSel, uint16_t addr, uint32_t value) {
  dpWriteFast(DP_SELECT, ((uint32_t)apSel << 24)|(addr & 0xF0));
  dpWriteFast(addr, value);
}

static void coreReadRegFast(struct Core core, uint16_t reg) {
  uint32_t addr = core.baddr + 4*reg;
  apWriteFast(core.ap, AP_TAR, addr);
  dpReadFast(AP_DRW);
}

void coreReadPcsrInit(uint64_t coresToSample) {
  sampleCores = coresToSample;

  startRec();

  fastBitsToRead = 0;

  lastIr = -1;

  for(int i = 0; i < numCores; i++) {
    if(sampleCores & (1 << i)) {
      if(cores[i].enabled) {
        if(cores[i].type == ARMV7A) {
          coreReadRegFast(cores[i], ARMV7A_PCSR);

        } else if(cores[0].type == ARMV8A) {
          coreReadRegFast(cores[i], ARMV8A_PCSR_L);
          coreReadRegFast(cores[i], ARMV8A_PCSR_H);
        }
      }
    }
  }

  if(cores[firstCore].type == ARMV8A) {
    coreReadRegFast(cores[firstCore], ARMV8A_PRSR);
  }

  storeRec();
}

bool coreReadPcsrFast(uint64_t *pcs, bool *halted) {
  *halted = false;

  uint32_t *words;

  if(!executeSeq()) return false;

  words = getWords();

  unsigned core = 0;

  for(unsigned i = 0; i < numCores; i++) {
    if(sampleCores & (1 << i)) {
      if(cores[i].enabled) {
        if(cores[i].type == ARMV7A) {
          uint32_t dbgpcsr = *words++;

          if(i < MAX_CORES) {
            if(dbgpcsr == 0xffffffff) {
              pcs[core] = dbgpcsr;
            } else {
              pcs[core] = calcOffset(dbgpcsr);
            }
          }
        } else if(cores[0].type == ARMV8A) {
          uint32_t dbgpcsrLow = *words++;
          uint32_t dbgpcsrHigh = *words++;

          uint64_t dbgpcsr = ((uint64_t)dbgpcsrHigh << 32) | dbgpcsrLow;

          if(i < MAX_CORES) {
            if(dbgpcsr == 0xffffffff) {
              pcs[core] = 0;
            } else {
              pcs[core] = calcOffset(dbgpcsr);
            }
          }
        }

      } else {
        pcs[core] = 0;
      }

      core++;
    }
  }

  for(; core < MAX_CORES; core++) {
    pcs[core] = 0;
  }

  if(cores[firstCore].type == ARMV8A) {
    uint32_t prsr = *words;
    *halted = prsr & (1 << 4);

  } else if(cores[firstCore].type == ARMV7A) {
    if(pcs[firstCore] == 0xffffffff) *halted = true;
    else *halted = false;
  }

  return true;
}

#endif

bool jtagInit(struct JtagDevice *devlist) {
  gotoResetThenIdle();

  // query chain
  if(!jtagQueryChain(devlist)) {
    return false;
  }

  { // find TAP
    bool found = false;

    for(int i = 0; i < numDevices; i++) {
      if((devices[i].idcode & 0x0fffffff) == ARM_TAP_IDCODE) {
        dpIdcode = devices[i].idcode;
        found = true;
        break;
      }
    }

    if(!found) {
      addLogLine("TAP not found\n");
      return false;
    }
  }

  return true;
}
