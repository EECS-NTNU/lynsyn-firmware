/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#include <stdio.h>

#include <em_usb.h>
#include <em_msc.h>
#include <inttypes.h>

#include "lynsyn_main.h"
#include "usb.h"
#include "adc.h"
#include "config.h"
#include "jtag.h"
#include "arm.h"
#include "jtag_lowlevel.h"
#include "../common/usbprotocol.h"

#include "descriptors.h"

#ifdef VERSION2
#include "fpga.h"
#endif

#ifdef __linux__
#include "linux.h"
#endif

///////////////////////////////////////////////////////////////////////////////

uint8_t firstCore = 0;
uint64_t markBp;

static struct SampleReplyPacket sample;

static uint32_t upgradeCrc;
static uint32_t flashPackageCounter;

uint8_t inBuffer[MAX_PACKET_SIZE + 3];

struct UsbTestReplyPacket usbTestReply __attribute__((__aligned__(4)));
struct CalInfoPacket calInfo __attribute__((__aligned__(4)));
struct InitReplyPacket initReply __attribute__((__aligned__(4)));
struct TestReplyPacket testReply __attribute__((__aligned__(4)));
struct TckReplyPacket tckReply __attribute__((__aligned__(4)));
struct ShiftReplyPacket shiftReply __attribute__((__aligned__(4)));
struct JtagInitReplyPacket jtagReply __attribute__((__aligned__(4)));

///////////////////////////////////////////////////////////////////////////////

static int usbDataSent(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining);
static int initSent(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining);
static void usbStateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);
int UsbDataReceived(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining);

///////////////////////////////////////////////////////////////////////////////

#ifndef __linux__
static const USBD_Callbacks_TypeDef callbacks = {
  .usbReset        = NULL,
  .usbStateChange  = usbStateChange,
  .setupCmd        = NULL,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

const USBD_Init_TypeDef initstruct = {
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

static void usbStateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState) {
  (void) oldState;
  if(newState == USBD_STATE_CONFIGURED) {
    USBD_Read(CDC_EP_DATA_OUT, inBuffer, MAX_PACKET_SIZE, UsbDataReceived);
  }
}

#endif

///////////////////////////////////////////////////////////////////////////////

static uint32_t crc32(uint32_t crc, uint32_t *data, int length) {
  for(int i = 0; i < length; i++ ) {
    crc = crc + data[i] ;
  }
  return crc ;
}

static void sendBuf(void *outBuffer, int length) {
#ifdef __linux__
  linux_usbWrite(outBuffer, length);
#else
	// send data to host 
  while(USBD_EpIsBusy(CDC_EP_DATA_IN));
  int ret = USBD_Write(CDC_EP_DATA_IN, (uint8_t*)outBuffer, length, usbDataSent);
  if(ret != USB_STATUS_OK) printf("Data error send: %d\n", ret);
#endif
}

///////////////////////////////////////////////////////////////////////////////

static void sendInitReply(void) {
  initReply.hwVersion = getUint32("hwver");
  initReply.swVersion = SW_VERSION;
  initReply.sensors = SENSORS;

#ifdef __linux__
  initReply.bootVersion = BOOT_VERSION_1_1;

  linux_usbWrite((uint8_t*)&initReply, sizeof(struct InitReplyPacket));
  initSent(USB_STATUS_OK, 1, 0);
#else
  initReply.bootVersion = (uint8_t)*(uint32_t*)FLASH_BOOT_VERSION;

  while(USBD_EpIsBusy(CDC_EP_DATA_IN));
  int ret = USBD_Write(CDC_EP_DATA_IN, &initReply, sizeof(struct InitReplyPacket) , initSent);
  if(ret != USB_STATUS_OK) printf("Data error send: %d\n", ret);
#endif
}

static void hwInit(struct HwInitRequestPacket *hwInitReq) {
  printf("Init non-volatile memory, version %x\n", hwInitReq->hwVersion);

  clearConfig();
  setUint32("hwver", hwInitReq->hwVersion);

#ifdef VERSION2
  setDouble("offset0", 0);
  setDouble("offset1", 0);
  setDouble("offset2", 0);
  setDouble("offset3", 0);
  setDouble("offset4", 0);
  setDouble("offset5", 0);
  setDouble("offset6", 0);

  setDouble("gain0", 1);
  setDouble("gain1", 1);
  setDouble("gain2", 1);
  setDouble("gain3", 1);
  setDouble("gain4", 1);
  setDouble("gain5", 1);
  setDouble("gain6", 1);

  setDouble("r0", hwInitReq->r[0]);
  setDouble("r1", hwInitReq->r[1]);
  setDouble("r2", hwInitReq->r[2]);
  setDouble("r3", hwInitReq->r[3]);
  setDouble("r4", hwInitReq->r[4]);
  setDouble("r5", hwInitReq->r[5]);
  setDouble("r6", hwInitReq->r[6]);

#else
  setDouble("offc00", 0);
  setDouble("offc10", 0);
  setDouble("offc20", 0);

  setDouble("gainc00", 1);
  setDouble("gainc10", 1);
  setDouble("gainc20", 1);

  setDouble("offv00", 0);
  setDouble("offv10", 0);
  setDouble("offv20", 0);

  setDouble("gainv00", 1);
  setDouble("gainv10", 1);
  setDouble("gainv20", 1);

  setDouble("r0", hwInitReq->r[0]);
  setDouble("r1", hwInitReq->r[1]);
  setDouble("r2", hwInitReq->r[2]);
#endif
}

static void setTck(struct SetTckRequestPacket *req) {
  uint32_t baud = 1000000000 / req->period;
  tckReply.period = 1000000000 / jtagSetBaudrate(baud);
  sendBuf(&tckReply, sizeof(struct TckReplyPacket));
}

static void shift(struct ShiftRequestPacket *req) {
  readWriteSeq(req->bits, req->tdi, req->tms, shiftReply.tdo);
  sendBuf(&shiftReply, sizeof(struct ShiftReplyPacket));
}

static void trst(struct TrstRequestPacket *req) {
  setTrst(req->val);
}

static void upgradeInit(struct RequestPacket *req) {
#ifndef __linux__
  MSC_Init();

	for(int i = FLASH_UPGRADE_START; i < FLASH_SIZE; i += 4096) {
    MSC_ErasePage((uint32_t*)i);
  }

  MSC_Deinit();

  upgradeCrc = 0;
  flashPackageCounter = 0;
#endif

  printf("Receiving new firmware...\n");
}

static void upgradeStore(struct UpgradeStoreRequestPacket *upgradeStoreRequest) {
#ifndef __linux__
  MSC_Init();

  uint32_t *destination = (uint32_t*)(FLASH_UPGRADE_START+flashPackageCounter*FLASH_BUFFER_SIZE);

  if(destination >= (uint32_t*)FLASH_UPGRADE_FLAG) return;

  MSC_WriteWord(destination, upgradeStoreRequest->data, FLASH_BUFFER_SIZE);

  upgradeCrc = crc32(upgradeCrc, (uint32_t*)upgradeStoreRequest->data, FLASH_BUFFER_SIZE / 4);

  MSC_Deinit();

  flashPackageCounter++;
#endif
}

static void finalizeUpgrade(struct UpgradeFinaliseRequestPacket *finalizeRequest) {
#ifndef __linux__
  printf("New firmware received\n");

  if(upgradeCrc != finalizeRequest->crc) {
    printf("Incorrect CRC, not upgrading\n");

  } else {
    MSC_Init();

    uint32_t buf = UPGRADE_MAGIC;

    MSC_WriteWord((uint32_t*)FLASH_UPGRADE_FLAG, &buf, 4);

    MSC_Deinit();

    DWT->CYCCNT = 0;
    while(DWT->CYCCNT < 24000000);

    NVIC_SystemReset();
  }
#endif
}

static void initJtag(struct JtagInitRequestPacket *jtagInitRequest) {
  printf("Init JTAG chain\n");
#ifdef VERSION2
  jtagInt();
#endif
  jtagReply.success = jtagInit(jtagInitRequest->jtagDevices);
  if(jtagReply.success) jtagReply.success = armInitCores(jtagInitRequest->armDevices);
  if(jtagReply.success) jtagReply.numCores = numCores;
  sendBuf(&jtagReply, sizeof(struct JtagInitReplyPacket));
}

static void setBreakpoint(struct BreakpointRequestPacket *bpReq) {
  switch(bpReq->bpType) {
    case BP_TYPE_START:
      printf("Set start BP %" PRIx64 "\n", bpReq->addr);
      setBp(START_BP, bpReq->addr);
      break;
    case BP_TYPE_STOP:
      printf("Set stop BP %" PRIx64 "\n", bpReq->addr);
      setBp(STOP_BP, bpReq->addr);
      break;
    case BP_TYPE_MARK:
      printf("Set mark BP %" PRIx64 "\n", bpReq->addr);
      markBp = bpReq->addr;
      break;
  }
}

static void startSampling(struct StartSamplingRequestPacket *startSamplingReq) {
  samplePc = startSamplingReq->cores;

  if(samplePc) {
    firstCore = 0;
    uint64_t c = startSamplingReq->cores;
    while(true) {
      if(c & 1) break;
      firstCore++;
      c >>= 1;
    }

    coreReadPcsrInit(startSamplingReq->cores);
  }

  bool useStartBp = startSamplingReq->flags & SAMPLING_FLAG_BP;
  useStopBp = !(startSamplingReq->flags & SAMPLING_FLAG_PERIOD);
  bool useMarkBp = startSamplingReq->flags & SAMPLING_FLAG_MARK;

  printf("Starting sample mode (%" PRIx64 ")\n", startSamplingReq->flags);

  setLed(0);

  if(useStartBp) {
    // run until start bp
    coresResume();

    // wait until we reach start bp
    bool halted = false;
    while(!halted) {
      coreHalted(firstCore, &halted);
    }

    // sampling starts here
    sampleMode = true;

    clearBp(START_BP);
    if(useMarkBp) setBp(MARK_BP, markBp);
    coresResume();

  } else {
    // sampling starts here
    sampleMode = true;
  }

  int64_t now = calculateTime();
  sampleStop = startSamplingReq->samplePeriod + now;

  printf("Sampling from %lld to %lld\n", now, sampleStop);
}

static void getSample(struct GetSampleRequestPacket *getSampleReq) {
  samplePc = getSampleReq->cores;
  bool average = getSampleReq->flags & SAMPLING_FLAG_AVERAGE;

  bool halted = false;
  bool sampleOk = true;

  if(samplePc) {
#ifdef VERSION2
    jtagInt();
#endif
    coreReadPcsrInit(getSampleReq->cores);
    sampleOk = coreReadPcsrFast(sample.pc, &halted);
#ifdef VERSION2
    jtagExt();
#endif
  }
  if(average) {
    getCurrentAvg(sample.channel);
  } else {
    //memcpy(sample.current, continuousCurrentInstant, sizeof(int16_t) * SENSORS);
    adcScan(sample.channel);
    adcScanWait();
  }

  sample.flags = 0;
  if(halted) sample.flags |= SAMPLE_REPLY_FLAG_HALTED;
  if(!sampleOk) sample.flags |= SAMPLE_REPLY_FLAG_INVALID;

  sendBuf(&sample, sizeof(struct SampleReplyPacket));
}

static void calSet(struct CalSetRequestPacket *cal) {
  printf("Setting ADC channel %d (val %f %f %d)\n", (int)cal->channel, cal->offset, cal->gain, cal->point);

  char configName[9];

#ifdef VERSION2
  snprintf(configName, 9, "offset%d", cal->channel);
  setDouble(configName, cal->offset);

  snprintf(configName, 9, "gain%d", cal->channel);
  setDouble(configName, cal->gain);

#else
  unsigned sensor = 9;
  char type = 'x';

  switch(cal->channel) {
    case 0:
      sensor = 2;
      type = 'c';
      break;
    case 1:
      sensor = 2;
      type = 'v';
      break;
    case 2:
      sensor = 1;
      type = 'c';
      break;
    case 3:
      sensor = 1;
      type = 'v';
      break;
    case 4:
      sensor = 0;
      type = 'c';
      break;
    case 5:
      sensor = 0;
      type = 'v';
      break;
  }

  snprintf(configName, 9, "off%c%d%d", type, sensor, cal->point);
  setDouble(configName, cal->offset);

  snprintf(configName, 9, "gain%c%d%d", type, sensor, cal->point);
  setDouble(configName, cal->gain);

  snprintf(configName, 9, "point%c%d%d", type, sensor, cal->point);
  setInt16(configName, (int16_t)cal->actual);
#endif
}

static void testProcedure(struct TestRequestPacket *testReq) {
  testReply.testStatus = 0;

  bool sendStatus = true;

  switch(testReq->testNum) {
    case TEST_USB: { // USB communication
      sendStatus = false;

      for(int i = 0; i < 256; i++) {
        usbTestReply.buf[i] = i;
      }

      sendBuf(&usbTestReply, sizeof(struct UsbTestReplyPacket));

      break;
    }
    case TEST_LEDS_ON: { // LEDs on
      setLed(0);
      //setLed(1);
      sendStatus = false;
      break;
    }
    case TEST_LEDS_OFF: { // LEDs off
      clearLed(0);
      //clearLed(1);
      sendStatus = false;
      break;
    }
  }

  if(sendStatus) {
    sendBuf(&testReply, sizeof(struct TestReplyPacket));
  }
}

///////////////////////////////////////////////////////////////////////////////

static int usbDataSent(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining) {
  (void)remaining;
  return USB_STATUS_OK;
}

static int initSent(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining) {
  (void)remaining;

#ifdef VERSION2
  calInfo.currentPoints[0] = 1;
  calInfo.currentPoints[1] = 1;
  calInfo.currentPoints[2] = 1;
  calInfo.currentPoints[3] = 1;
  calInfo.currentPoints[4] = 1;
  calInfo.currentPoints[5] = 1;
  calInfo.currentPoints[6] = 1;

  calInfo.offsetCurrent[0][0] = getDouble("offset0");
  calInfo.offsetCurrent[1][0] = getDouble("offset1");
  calInfo.offsetCurrent[2][0] = getDouble("offset2");
  calInfo.offsetCurrent[3][0] = getDouble("offset3");
  calInfo.offsetCurrent[4][0] = getDouble("offset4");
  calInfo.offsetCurrent[5][0] = getDouble("offset5");
  calInfo.offsetCurrent[6][0] = getDouble("offset6");

  calInfo.gainCurrent[0][0] = getDouble("gain0");
  calInfo.gainCurrent[1][0] = getDouble("gain1");
  calInfo.gainCurrent[2][0] = getDouble("gain2");
  calInfo.gainCurrent[3][0] = getDouble("gain3");
  calInfo.gainCurrent[4][0] = getDouble("gain4");
  calInfo.gainCurrent[5][0] = getDouble("gain5");
  calInfo.gainCurrent[6][0] = getDouble("gain6");

  calInfo.pointCurrent[0][0] = 0;
  calInfo.pointCurrent[1][0] = 0;
  calInfo.pointCurrent[2][0] = 0;
  calInfo.pointCurrent[3][0] = 0;
  calInfo.pointCurrent[4][0] = 0;
  calInfo.pointCurrent[5][0] = 0;
  calInfo.pointCurrent[6][0] = 0;

  if(configExists("r0")) calInfo.r[0] = getDouble("r0"); else calInfo.r[0] = 0.025;
  if(configExists("r1")) calInfo.r[1] = getDouble("r1"); else calInfo.r[1] = 0.05;
  if(configExists("r2")) calInfo.r[2] = getDouble("r2"); else calInfo.r[2] = 0.05;
  if(configExists("r3")) calInfo.r[3] = getDouble("r3"); else calInfo.r[3] = 0.1;
  if(configExists("r4")) calInfo.r[4] = getDouble("r4"); else calInfo.r[4] = 0.1;
  if(configExists("r5")) calInfo.r[5] = getDouble("r5"); else calInfo.r[5] = 1;
  if(configExists("r6")) calInfo.r[6] = getDouble("r6"); else calInfo.r[6] = 10;
  
#else
  char configName[9];

  for(int sensor = 0; sensor < MAX_SENSORS; sensor++) {
    calInfo.currentPoints[sensor] = 0;
    calInfo.voltagePoints[sensor] = 0;

    for(int pointNum = 0; pointNum < MAX_POINTS; pointNum++) {
      snprintf(configName, 9, "offc%d%d", sensor, pointNum);
      if(configExists(configName)) {
        calInfo.offsetCurrent[sensor][pointNum] = getDouble(configName);

        snprintf(configName, 9, "gainc%d%d", sensor, pointNum);
        calInfo.gainCurrent[sensor][pointNum] = getDouble(configName);
      
        snprintf(configName, 9, "pointc%d%d", sensor, pointNum);
        calInfo.pointCurrent[sensor][pointNum] = getInt16(configName);

        calInfo.currentPoints[sensor]++;
      } else {
        calInfo.offsetCurrent[sensor][pointNum] = 0;
        break;
      }
    }

    for(int pointNum = 0; pointNum < MAX_POINTS; pointNum++) {
      snprintf(configName, 9, "offv%d%d", sensor, pointNum);
      if(configExists(configName)) {
        calInfo.offsetVoltage[sensor][pointNum] = getDouble(configName);

        snprintf(configName, 9, "gainv%d%d", sensor, pointNum);
        calInfo.gainVoltage[sensor][pointNum] = getDouble(configName);
      
        snprintf(configName, 9, "pointv%d%d", sensor, pointNum);
        calInfo.pointVoltage[sensor][pointNum] = getInt16(configName);

        calInfo.voltagePoints[sensor]++;
      } else {
        calInfo.offsetVoltage[sensor][pointNum] = 0;
        break;
      }

    }
  }

  calInfo.r[0] = getDouble("r0");
  calInfo.r[1] = getDouble("r1");
  calInfo.r[2] = getDouble("r2");
#endif

  sendBuf(&calInfo, sizeof(struct CalInfoPacket));

  return USB_STATUS_OK;
}

int UsbDataReceived(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining) {

  if((status == USB_STATUS_OK) && (xf > 0)) {

    uint8_t *ptr = inBuffer;

    while(xf) {
      int currentPacketSize = sizeof(struct RequestPacket);
      struct RequestPacket *req = (struct RequestPacket *)ptr;

      switch(req->cmd) {
        case USB_CMD_INIT:             currentPacketSize = sizeof(struct RequestPacket);                break;
        case USB_CMD_HW_INIT:          currentPacketSize = sizeof(struct HwInitRequestPacket);          break;
        case USB_CMD_UPGRADE_INIT:     currentPacketSize = sizeof(struct RequestPacket);                break;
        case USB_CMD_UPGRADE_STORE:    currentPacketSize = sizeof(struct UpgradeStoreRequestPacket);    break;
        case USB_CMD_UPGRADE_FINALISE: currentPacketSize = sizeof(struct UpgradeFinaliseRequestPacket); break;
        case USB_CMD_JTAG_INIT:        currentPacketSize = sizeof(struct JtagInitRequestPacket);        break;
        case USB_CMD_BREAKPOINT:       currentPacketSize = sizeof(struct BreakpointRequestPacket);      break;
        case USB_CMD_START_SAMPLING:   currentPacketSize = sizeof(struct StartSamplingRequestPacket);   break;
        case USB_CMD_GET_SAMPLE:       currentPacketSize = sizeof(struct GetSampleRequestPacket);       break;
        case USB_CMD_CAL_SET:          currentPacketSize = sizeof(struct CalSetRequestPacket);          break;
        case USB_CMD_TCK:              currentPacketSize = sizeof(struct SetTckRequestPacket);          break;
        case USB_CMD_SHIFT:            currentPacketSize = sizeof(struct ShiftRequestPacket);           break;
        case USB_CMD_TRST:             currentPacketSize = sizeof(struct TrstRequestPacket);            break;
        case USB_CMD_TEST:             currentPacketSize = sizeof(struct TestRequestPacket);            break;
        default: {
          printf("USB error %x\n", (unsigned)req->cmd);
#ifndef __linux__
          USBD_Read(CDC_EP_DATA_OUT, inBuffer, MAX_PACKET_SIZE, UsbDataReceived);
#endif
          return USB_STATUS_OK;
        }
      }

      if(currentPacketSize > xf) {
        printf("USB error %x %d > %d\n", (unsigned)req->cmd, (unsigned)currentPacketSize, (unsigned)xf);
#ifndef __linux__
        USBD_Read(CDC_EP_DATA_OUT, inBuffer, MAX_PACKET_SIZE, UsbDataReceived);
#endif
        return USB_STATUS_OK;
      }

      switch(req->cmd) {
        case USB_CMD_INIT:             sendInitReply();                                             break;
        case USB_CMD_HW_INIT:          hwInit((struct HwInitRequestPacket *)req);                   break;
        case USB_CMD_UPGRADE_INIT:     upgradeInit(req);                                            break;
        case USB_CMD_UPGRADE_STORE:    upgradeStore((struct UpgradeStoreRequestPacket *)req);       break;
        case USB_CMD_UPGRADE_FINALISE: finalizeUpgrade((struct UpgradeFinaliseRequestPacket *)req); break;
        case USB_CMD_JTAG_INIT:        initJtag((struct JtagInitRequestPacket *)req);               break;
        case USB_CMD_BREAKPOINT:       setBreakpoint((struct BreakpointRequestPacket *)req);        break;
        case USB_CMD_START_SAMPLING:   startSampling((struct StartSamplingRequestPacket *)req);     break;
        case USB_CMD_GET_SAMPLE:       getSample((struct GetSampleRequestPacket *)req);             break;
        case USB_CMD_CAL_SET:          calSet((struct CalSetRequestPacket *)req);                   break;
        case USB_CMD_TCK:              setTck((struct SetTckRequestPacket *)req);                   break;
        case USB_CMD_SHIFT:            shift((struct ShiftRequestPacket *)req);                     break;
        case USB_CMD_TRST:             trst((struct TrstRequestPacket *)req);                       break;
        case USB_CMD_TEST:             testProcedure((struct TestRequestPacket *)req);              break;
      }          

      xf -= currentPacketSize;
      ptr += currentPacketSize;
    }
  }

#ifndef __linux__
  USBD_Read(CDC_EP_DATA_OUT, inBuffer, MAX_PACKET_SIZE, UsbDataReceived);
#endif

  return USB_STATUS_OK;
}

void usbInit(void) {
#ifndef __linux__
  USBD_Init(&initstruct);
#endif
}

bool sendSamples(struct SampleReplyPacket *sample, unsigned n) {
	// send samples to host 
#ifdef __linux__
  linux_usbWrite((uint8_t*)sample, n * sizeof(struct SampleReplyPacket));
#else
  while(USBD_EpIsBusy(CDC_EP_DATA_IN));

  int ret = USBD_Write(CDC_EP_DATA_IN, (uint8_t*)sample, MAX_SAMPLES * sizeof(struct SampleReplyPacket), usbDataSent);
  if(ret != USB_STATUS_OK) printf("Data error send: %d\n", ret);
#endif

  return true;
}
