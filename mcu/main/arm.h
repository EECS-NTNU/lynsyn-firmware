/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef ARM_H
#define ARM_H

#include "lynsyn_main.h"
#include "zynq.h"
#include "arm_defines.h"
#include "../common/usbprotocol.h"

enum {
  ARMV7A,
  ARMV8A
};

struct Core {
  unsigned type;
  unsigned ap;
  uint32_t baddr;
  bool enabled;
};

///////////////////////////////////////////////////////////////////////////////
// public functions

// call this at least every time a new board has been plugged in
bool armInitCores(struct ArmDevice *armList); 

void coreReadPcsrInit(uint64_t cores);
bool coreReadPcsrFast(uint64_t *pcs, bool *halted);

bool coresResume(void);
bool coreSetBp(unsigned core, unsigned bpNum, uint64_t addr);
bool coreClearBp(unsigned core, unsigned bpNum);

bool setBp(unsigned bpNum, uint64_t addr);
bool clearBp(unsigned bpNum);

bool coreReadStatus(unsigned core, uint8_t *value);
bool readPc(unsigned core, uint64_t *value);

bool coreHalted(unsigned core, bool *value);

uint32_t extractWord(unsigned *pos, uint8_t *buf);
uint8_t extractAck(unsigned *pos, uint8_t *buf);

uint64_t calcOffset(uint64_t dbgpcsr);

///////////////////////////////////////////////////////////////////////////////

extern unsigned numCores;
extern unsigned numEnabledCores;
extern struct Core cores[];

#endif
