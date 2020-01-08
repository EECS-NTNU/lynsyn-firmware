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

enum {
  CORTEX_A9,
  CORTEX_A53,
  CORTEX_A57,
  DENVER_2
};

struct Core {
  unsigned type;
  unsigned core;
  unsigned ap;
  uint32_t baddr;
  bool enabled;
};

///////////////////////////////////////////////////////////////////////////////
// public functions

bool armInitCores(struct JtagDevice *devices); // call this at least every time a new board has been plugged in

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
