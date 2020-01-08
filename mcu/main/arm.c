/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *  Lynsyn is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Lynsyn is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Lynsyn.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <em_usart.h>

#include "arm.h"
#include "jtag.h"

///////////////////////////////////////////////////////////////////////////////

static unsigned apSelMem = 0;
static bool mem64 = false;
unsigned numCores;
unsigned numEnabledCores;
struct Core cores[MAX_CORES];

///////////////////////////////////////////////////////////////////////////////

uint32_t extractWord(unsigned *pos, uint8_t *buf) {
  unsigned byte = *pos / 8;
  unsigned bit = *pos % 8;

  *pos += 32;

  switch(bit) {
    case 0: return (buf[byte+3] << 24) | (buf[byte+2] << 16) | (buf[byte+1] << 8) | buf[byte];
    case 1: return ((buf[byte+4] & 0x01) << 31) | (buf[byte+3] << 23) | (buf[byte+2] << 15) | (buf[byte+1] << 7) | (buf[byte] >> 1);
    case 2: return ((buf[byte+4] & 0x03) << 30) | (buf[byte+3] << 22) | (buf[byte+2] << 14) | (buf[byte+1] << 6) | (buf[byte] >> 2);
    case 3: return ((buf[byte+4] & 0x07) << 29) | (buf[byte+3] << 21) | (buf[byte+2] << 13) | (buf[byte+1] << 5) | (buf[byte] >> 3);
    case 4: return ((buf[byte+4] & 0x0f) << 28) | (buf[byte+3] << 20) | (buf[byte+2] << 12) | (buf[byte+1] << 4) | (buf[byte] >> 4);
    case 5: return ((buf[byte+4] & 0x1f) << 27) | (buf[byte+3] << 19) | (buf[byte+2] << 11) | (buf[byte+1] << 3) | (buf[byte] >> 5);
    case 6: return ((buf[byte+4] & 0x3f) << 26) | (buf[byte+3] << 18) | (buf[byte+2] << 10) | (buf[byte+1] << 2) | (buf[byte] >> 6);
    case 7: return ((buf[byte+4] & 0x7f) << 25) | (buf[byte+3] << 17) | (buf[byte+2] <<  9) | (buf[byte+1] << 1) | (buf[byte] >> 7);
  }

  return 0;
}

uint8_t extractAck(unsigned *pos, uint8_t *buf) {
  unsigned byte = *pos / 8;
  unsigned bit = *pos % 8;

  *pos += 3;

  switch(bit) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5: return (buf[byte] >> bit) & 0x7;
    case 6: return ((buf[byte+1] & 0x1) << 2) | ((buf[byte] >> 6) & 0x3);
    case 7: return ((buf[byte+1] & 0x3) << 1) | ((buf[byte] >> 7) & 0x1);
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ARM DP

static bool dpWrite(uint16_t addr, uint32_t value) {
  return jtagDpLowAccess(LOW_WRITE, addr, value, NULL);
}

static bool dpRead(uint16_t addr, uint32_t *value) {
  if(!jtagDpLowAccess(LOW_READ, addr, 0, NULL)) return false;
  return jtagDpLowAccess(LOW_READ, DP_RDBUFF, 0, value);
}

static bool dpAbort(uint32_t abort) {
  uint8_t request_buf[8];
  memset(request_buf, 0, 8);

  request_buf[4] = (abort & 0xE0000000) >> 29;
  uint32_t tmp = (abort << 3);
  request_buf[3] = (tmp & 0xFF000000) >> 24;
  request_buf[2] = (tmp & 0x00FF0000) >> 16;
  request_buf[1] = (tmp & 0x0000FF00) >> 8;
  request_buf[0] = (tmp & 0x000000FF);

  jtagWriteIr(dpIdcode, ARM_IR_ABORT);
  if(!jtagReadWriteDr(dpIdcode, request_buf, NULL, 35)) return false;

  return true;
}

static bool dpInit(void) {
  if(!dpAbort(DP_ABORT_DAPABORT)) return false;

  uint32_t ctrlstat;
  if(!dpRead(DP_CTRLSTAT, &ctrlstat)) return false;
  
  /* Write request for system and debug power up */
  if(!dpWrite(DP_CTRLSTAT,
              ctrlstat |= DP_CTRLSTAT_CSYSPWRUPREQ |
              DP_CTRLSTAT_CDBGPWRUPREQ)) {
    return false;
  }

  /* Wait for acknowledge */
  do {
    if(!dpRead(DP_CTRLSTAT, &ctrlstat)) return false;
  } while((ctrlstat & (DP_CTRLSTAT_CSYSPWRUPACK | DP_CTRLSTAT_CDBGPWRUPACK)) !=
          (DP_CTRLSTAT_CSYSPWRUPACK | DP_CTRLSTAT_CDBGPWRUPACK));

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// ARM AP

static bool apWrite(unsigned apSel, uint16_t addr, uint32_t value) {
  if(!dpWrite(DP_SELECT, ((uint32_t)apSel << 24)|(addr & 0xF0))) return false;
  return dpWrite(addr, value);
}

static bool apRead(unsigned apSel, uint16_t addr, uint32_t *value) {
  if(!dpWrite(DP_SELECT, ((uint32_t)apSel << 24)|(addr & 0xF0))) return false;
  return dpRead(addr, value);
}

///////////////////////////////////////////////////////////////////////////////
// ARM AXI

/* static bool axiReadMem(uint64_t addr, uint32_t *value) { */
/*   if(!apWrite(apSelMem, AP_TAR, addr & 0xffffffff)) return false; */
/*   if(mem64) if(!apWrite(apSelMem, AP_TAR_HI, addr >> 32)) return false; */
/*   return dpRead(AP_DRW, value); */
/* } */

/* static bool axiWriteMem(uint64_t addr, uint32_t val) { */
/*   if(!apWrite(apSelMem, AP_TAR, addr & 0xffffffff)) return false; */
/*   if(mem64) if(!apWrite(apSelMem, AP_TAR_HI, addr >> 32)) return false; */
/*   return dpWrite(AP_DRW, val); */
/* } */

///////////////////////////////////////////////////////////////////////////////
// ARM Core

static bool coreWriteReg(struct Core *core, uint16_t reg, uint32_t val) {
  uint32_t addr = core->baddr + 4*reg;
  if(!apWrite(core->ap, AP_TAR, addr)) return false;
  return dpWrite(AP_DRW, val);
}

static bool coreReadReg(struct Core *core, uint16_t reg, uint32_t *value) {
  uint32_t addr = core->baddr + 4*reg;
  if(!apWrite(core->ap, AP_TAR, addr)) return false;
  return dpRead(AP_DRW, value);
}

bool coreReadStatus(unsigned core, uint8_t *value) {
  uint32_t scr;
  if(!coreReadReg(&cores[core], ARMV8A_SCR, &scr)) return false;
  *value = scr & 0x3f;
  return true;
}

bool readPc(unsigned core, uint64_t *value) {
  if(cores[core].enabled) {
    if(cores[core].type == ARMV7A) {
      uint32_t dbgdscr;

      if(!coreReadReg(&cores[0], ARMV7A_DSCR, &dbgdscr)) return false;
      dbgdscr |= DSCR_ITREN;
      if(!coreWriteReg(&cores[0], ARMV7A_DSCR, dbgdscr)) return false;

      if(!coreWriteReg(&cores[core], ARMV7A_ITR, 0xe1a0000f)) return false;
      uint32_t regno = 0;
      uint32_t instr = MCR | DTRTXint | ((regno & 0xf) << 12);
      if(!coreWriteReg(&cores[core], ARMV7A_ITR, instr)) return false;

      uint32_t dtrtx;
      if(!coreReadReg(&cores[core], ARMV7A_DTRTX, &dtrtx)) return false;
      *value = dtrtx - 8;

      return true;

    } else if(cores[core].type == ARMV8A) {
      if(!coreWriteReg(&cores[core], ARMV8A_ITR, ARMV8A_MRS_DLR(0))) return false;
      if(!coreWriteReg(&cores[core], ARMV8A_ITR, ARMV8A_MSR_GP(SYSTEM_DBG_DBGDTR_EL0, 0))) return false;

      uint32_t low;
      uint32_t high;

      if(!coreReadReg(&cores[core], ARMV8A_DTRRX, &high)) return false;
      if(!coreReadReg(&cores[core], ARMV8A_DTRTX, &low)) return false;

      *value = ((uint64_t)high << 32) | low;

      return true;
    }
  }

  return false;
}

uint64_t calcOffset(uint64_t dbgpcsr) {
  return dbgpcsr & ~3;
}

static bool coreReadPcsr(struct Core *core, uint64_t *value) {
  uint64_t dbgpcsr = 0;

  if(core->type == ARMV7A) {
    uint32_t pcsr;
    if(!coreReadReg(core, ARMV7A_PCSR, &pcsr)) return false;
    dbgpcsr = pcsr;

  } else if(core->type == ARMV8A) {
    if(core->enabled) {
      uint32_t low, high;
      if(!coreReadReg(core, ARMV8A_PCSR_L, &low)) return false;
      if(!coreReadReg(core, ARMV8A_PCSR_H, &high)) return false;
      dbgpcsr = ((uint64_t)high << 32) | low;
    } else {
      dbgpcsr = 0xffffffff;
    }
  }

  if (dbgpcsr == 0xffffffff) {
    // This happens when
    // - Non-invasive debug is disabled
    // - Processor is in a mode or state where non-invasive debug is not permitted
    // - Processor is in Debug state.
    *value = dbgpcsr;
    return true;

  } else {
    *value = calcOffset(dbgpcsr);
    return true;
  }

  return false;
}



bool coreSetBp(unsigned core, unsigned bpNum, uint64_t addr) {
  if(cores[core].type == ARMV7A) {
    if(!coreWriteReg(&cores[core], ARMV7A_BVR(bpNum), addr & ~3)) return false;
    if(!coreWriteReg(&cores[core], ARMV7A_BCR(bpNum), BCR_BAS_ANY | BCR_EN)) return false;

  } else if(cores[core].type == ARMV8A) {
    if(!coreWriteReg(&cores[core], ARMV8A_BVR_L(bpNum), addr & 0xfffffffc)) return false;
    if(!coreWriteReg(&cores[core], ARMV8A_BVR_H(bpNum), (addr >> 32))) return false;
    if(!coreWriteReg(&cores[core], ARMV8A_BCR(bpNum), (1 << 13) | (3 << 1) | BCR_BAS_ANY | BCR_EN)) return false;
    uint32_t scr;
    if(!coreReadReg(&cores[core], ARMV8A_SCR, &scr)) return false;
    if(!coreWriteReg(&cores[core], ARMV8A_SCR, scr | (0x1 << 14))) return false;
  }

  return true;
}

bool coreClearBp(unsigned core, unsigned bpNum) {
  if(cores[core].type == ARMV7A) {
    if(!coreWriteReg(&cores[core], ARMV7A_BCR(bpNum), 0)) return false;
  } else if(cores[core].type == ARMV8A) {
    if(!coreWriteReg(&cores[core], ARMV8A_BCR(bpNum), 0)) return false;
  }
  return true;
}

bool setBp(unsigned bpNum, uint64_t addr) {
  for(unsigned j = 0; j < numCores; j++) {
    if(cores[j].enabled) {
      if(!coreSetBp(j, bpNum, addr)) return false;
    }
  }
  return true;
}

bool clearBp(unsigned bpNum) {
  for(unsigned j = 0; j < numCores; j++) {
    if(cores[j].enabled) {
      if(!coreClearBp(j, bpNum)) return false;
    }
  }
  return true;
}

bool coresResume(void) {
  // FIXME: Does not work if there are several different types of cores in the system

  if(cores[0].type == ARMV7A) {
    uint32_t dbgdscr;

    // enable halting debug mode for controlling breakpoints
    if(!coreReadReg(&cores[0], ARMV7A_DSCR, &dbgdscr)) return false;
    dbgdscr |= DSCR_HDBGEN;
    dbgdscr &= ~DSCR_ITREN;
    if(!coreWriteReg(&cores[0], ARMV7A_DSCR, dbgdscr)) return false;
    
    // clear sticky error and resume
    if(!coreWriteReg(&cores[0], ARMV7A_DRCR, DRCR_CSE | DRCR_RRQ)) return false;
    
    // wait for restart
    while(1) {
      if(!coreReadReg(&cores[0], ARMV7A_DSCR, &dbgdscr)) return false;
      if(dbgdscr & DSCR_RESTARTED) {
        break;
      }
      if(!coreWriteReg(&cores[0], ARMV7A_DRCR, DRCR_CSE | DRCR_RRQ)) return false;
    }

  } else if(cores[0].type == ARMV8A) {
    bool halted;
    if(!coreHalted(0, &halted)) return false;
    if(halted) {
      for(int i = 0; i < numCores; i++) {
        if(cores[i].enabled) {
          /* ack */
          if(!coreWriteReg(&cores[i], ARMV8A_CTIINTACK, HALT_EVENT_BIT)) return false;
        }
      }

      /* Pulse Channel 0 */
      if(!coreWriteReg(&cores[0], ARMV8A_CTIAPPPULSE, CHANNEL_0)) return false;
      
      /* Poll until restarted */
      uint32_t prsr;
      do {
        if(!coreReadReg(&cores[0], ARMV8A_PRSR, &prsr)) return false;
      } while(!(prsr & (1<<11)));
    }
  }

  return true;
}

bool coreHalted(unsigned core, bool *value) {
  if(cores[core].type == ARMV7A) {
    uint64_t pcsr;
    if(!coreReadPcsr(&cores[core], &pcsr)) return false;
    *value = pcsr == 0xffffffff;

  } else if(cores[core].type == ARMV8A) {
    uint32_t prsr;
    if(!coreReadReg(&cores[core], ARMV8A_PRSR, &prsr)) return false;
    *value = prsr & (1 << 4);
  }

  return true;
}

bool coreInitA9(unsigned apSel, uint32_t baddr, struct Core *core) {
  core->type = ARMV7A;
  core->core = CORTEX_A9;
  core->ap = apSel;
  core->baddr = baddr;
  core->enabled = true;
  //printf("        Enabled\n");

  return true;
}

bool coreInitArmV8(unsigned apSel, uint32_t baddr, struct Core *core) {
  core->type = ARMV8A;
  core->ap = apSel;
  core->baddr = baddr;
  uint32_t prsr;
  if(!coreReadReg(core, ARMV8A_PRSR, &prsr)) return false;
  core->enabled = prsr & 1;

  if(core->enabled) {
    //printf("        Enabled\n");
    
    /* enable CTI */
    if(!coreWriteReg(core, ARMV8A_CTICONTROL, 1)) return false;
    
    /* send channel 0 and 1 out to other PEs */
    if(!coreWriteReg(core, ARMV8A_CTIGATE, CHANNEL_1 | CHANNEL_0)) return false;
    
    /* generate channel 1 event on halt trigger */
    if(!coreWriteReg(core, ARMV8A_CTIINEN(HALT_EVENT), CHANNEL_1)) return false;
    
    /* halt on channel 1 */
    if(!coreWriteReg(core, ARMV8A_CTIOUTEN(HALT_EVENT), CHANNEL_1)) return false;
    
    /* restart on channel 0 */
    if(!coreWriteReg(core, ARMV8A_CTIOUTEN(RESTART_EVENT), CHANNEL_0)) return false;
    
  } else {
    //printf("        Disabled\n");
  }

  return true;
}

bool coreInitA53(unsigned apSel, uint32_t baddr, struct Core *core) {
  core->core = CORTEX_A53;
  return coreInitArmV8(apSel, baddr, core);
}

bool coreInitA57(unsigned apSel, uint32_t baddr, struct Core *core) {
  core->core = CORTEX_A57;
  return coreInitArmV8(apSel, baddr, core);
}

bool coreInitDenver2(unsigned apSel, uint32_t baddr, struct Core *core) {
  core->core = DENVER_2;
  return coreInitArmV8(apSel, baddr, core);
}

///////////////////////////////////////////////////////////////////////////////

bool parseDebugEntry(unsigned apSel, uint32_t compBase) {
  
  // read CIDR1
  uint32_t addr = compBase + 0xff4;
  if(!apWrite(apSel, AP_TAR, addr)) return false;
  uint32_t cidr1;
  if(!dpRead(AP_DRW, &cidr1)) return false;
  
  if((cidr1 & 0xf0) == 0x10) { // is a ROM
    bool done = false;
    uint32_t entryAddr = compBase;

    //printf("  Got ROM at %x\n", (unsigned)compBase);

    while(!done) {
      // read entry
      if(!apWrite(apSel, AP_TAR, entryAddr)) return false;
      uint32_t entry;
      if(!dpRead(AP_DRW, &entry)) return false;
      
      if(!entry) done = true;

      // else
      if(entry & 1) { // entry is valid
        if(!parseDebugEntry(apSel, compBase + (entry & 0xfffff000))) return false;
      }

      entryAddr += 4;
    }

  } else if((cidr1 & 0xf0) == 0x90) { // is debug component
    // read PIDR

    uint32_t addr0 = compBase + 0xfe0;
    if(!apWrite(apSel, AP_TAR, addr0)) return false;
    uint32_t pidr0;
    if(!dpRead(AP_DRW, &pidr0)) return false;
    
    uint32_t addr1 = compBase + 0xfe4;
    if(!apWrite(apSel, AP_TAR, addr1)) return false;
    uint32_t pidr1;
    if(!dpRead(AP_DRW, &pidr1)) return false;
    
    uint32_t addr2 = compBase + 0xfe8;
    if(!apWrite(apSel, AP_TAR, addr2)) return false;
    uint32_t pidr2;
    if(!dpRead(AP_DRW, &pidr2)) return false;
    
    uint32_t addr4 = compBase + 0xfd0;
    if(!apWrite(apSel, AP_TAR, addr4)) return false;
    uint32_t pidr4;
    if(!dpRead(AP_DRW, &pidr4)) return false;
    
    //printf("    Got Entry at %x: %x %x %x %x\n", (unsigned)compBase, (unsigned)pidr0, (unsigned)pidr1, (unsigned)pidr2, (unsigned)pidr4);

    /* if(((pidr0 & 0xff) == 0x15) && // found a Cortex R5 */
    /*    ((pidr1 & 0xff) == 0xbc) && */
    /*    ((pidr2 & 0x0f) == 0xb) && */
    /*    ((pidr4 & 0x0f) == 0x4)) { */

    /*   cores[numCores] = coreInitR5(apSel, compBase); */
    /*   if(cores[numCores].enabled) numEnabledCores++; */

    /*   numCores++; */

    /* } else */

    if(((pidr0 & 0xff) == 0x9) && // found a Cortex A9
       ((pidr1 & 0xff) == 0xbc) &&
       ((pidr2 & 0x0f) == 0xb) &&
       ((pidr4 & 0x0f) == 0x4)) {

      printf("      Found Cortex A9\n");

      if(!coreInitA9(apSel, compBase, &cores[numCores])) return false;
      if(cores[numCores].enabled) numEnabledCores++;

      numCores++;

    } else if(((pidr0 & 0xff) == 0x3) && // found a Cortex A53
             ((pidr1 & 0xff) == 0xbd) &&
             ((pidr2 & 0x0f) == 0xb) &&
             ((pidr4 & 0x0f) == 0x4)) {

      printf("      Found Cortex A53\n");

      if(!coreInitA53(apSel, compBase, &cores[numCores])) return false;
      if(cores[numCores].enabled) numEnabledCores++;

      numCores++;

    } else if(((pidr0 & 0xff) == 0x7) && // found a Cortex A57
             ((pidr1 & 0xff) == 0xbd) &&
             ((pidr2 & 0x0f) == 0xb) &&
             ((pidr4 & 0x0f) == 0x4)) {

      printf("      Found Cortex A57\n");

      if(!coreInitA57(apSel, compBase, &cores[numCores])) return false;
      if(cores[numCores].enabled) numEnabledCores++;

      numCores++;

    /* } else if(((pidr0 & 0xff) == 0x2) && // found a Denver 2 */
    /*          ((pidr1 & 0xff) == 0xb3) && */
    /*          ((pidr2 & 0x0f) == 0xe) && */
    /*          ((pidr4 & 0x0f) == 0x3)) { */

    /*   printf("      Found Denver 2\n"); */

    /*   if(!coreInitDenver2(apSel, compBase, &cores[numCores])) return false; */
    /*   if(cores[numCores].enabled) numEnabledCores++; */

    /*   numCores++; */
    }
  }

  return true;
}

bool armInitCores(struct JtagDevice *devlist) {

  if(!jtagInit(devlist)) return false;

  if(!dpInit()) return false;

  { // find APs and cores
    numCores = 0;
    numEnabledCores = 0;

    for(int i = 0; i < MAX_APS; i++) {
      uint32_t idr;
      if(!apRead(i, AP_IDR, &idr)) return false;
      uint32_t base;
      if(!apRead(i, AP_BASE, &base)) return false;
      uint32_t cfg;
      if(!apRead(i, AP_CFG, &cfg)) return false;
      
      if((idr & 0x0fffff0f) == IDR_APB_AP) {
        //printf("Found APB AP (%d) idr %x base %x cfg %x\n", i, (unsigned)idr, (unsigned)base, (unsigned)cfg);

        if(base == ~0) { // legacy format, not present
          
        } else if((base & 0x3) == 0) { // legacy format, present
          if(!apWrite(i, AP_CSW, 0x80000042)) return false;
          if(!parseDebugEntry(i, base & 0xfffff000)) return false;
          
        } else if((base & 0x3) == 0x3) { // debug entry is present
          if(!apWrite(i, AP_CSW, 0x80000042)) return false;
          if(!parseDebugEntry(i, base & 0xfffff000)) return false;
        }

      } else if((idr & 0x0fffff0f) == IDR_AXI_AP) {
        //printf("Found AXI AP (%d) idr %x base %x cfg %x\n", i, (unsigned)idr, (unsigned)base, (unsigned)cfg);

        apSelMem = i;
        mem64 = cfg & 2;
        if(mem64) {
          //printf("  64 bit AXI memory space\n");
        }

      } else if((idr & 0x0fffff0f) == IDR_AHB_AP) {
        //printf("Found AHB AP (%d)\n", i);

        apSelMem = i;
        mem64 = false;

      } else if(idr != 0) {
        //printf("Found unknown AP %x (%d)\n", (unsigned)idr, i);
      }
    }
  }

  if(numCores == 0) {
    //printf("No cores found\n");
    return false;
  }

  // get num breakpoints
  unsigned numBps = 0;
  if(cores[0].type == ARMV7A) {
    uint32_t didr;
    if(!coreReadReg(&cores[0], ARMV7A_DIDR, &didr)) return false;
    numBps = ((didr >> 24) & 0xf) + 1;
  } else if(cores[0].type == ARMV8A) {
    uint32_t dfr0;
    if(!coreReadReg(&cores[0], ARMV8A_DFR0, &dfr0)) return false;
    numBps = ((dfr0 >> 12) & 0xf) + 1;
  }

  // clear all breakpoints
  for(unsigned j = 0; j < numCores; j++) {
    if(cores[j].enabled) {
      for(unsigned i = 0; i < numBps; i++) {
        if(!coreClearBp(j, i)) return false;
      }
    }
  }

  return true;
}

