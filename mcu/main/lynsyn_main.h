/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018 Asbjørn Djupdal, NTNU, TULIPP EU Project
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

#ifndef LYNSYN_MAIN_H
#define LYNSYN_MAIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#include "../common/lynsyn_fw.h"
#include "../common/usbprotocol.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_system.h"
#include "em_chip.h"

#ifdef __linux__
#include "linux/linux.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// firmware settings

//#define DUMP_PINS
#define CAL_AVERAGE_SAMPLES 256

///////////////////////////////////////////////////////////////////////////////
// global functions

void panic(const char *fmt, ...);
int64_t calculateTime();
void getCurrentAvg(int16_t *sampleBuffer);

///////////////////////////////////////////////////////////////////////////////
// global variables

extern volatile bool sampleMode;
extern volatile bool samplePc;
extern volatile bool useStopBp;
extern volatile int64_t sampleStop;
extern uint8_t firstCore;
extern uint64_t markBp;
extern uint64_t continuousCurrentAcc[CHANNELS];
extern int continuousSamplesSinceLast[CHANNELS];
extern int16_t continuousCurrentInstant[CHANNELS];
extern int16_t continuousCurrentAvg[CHANNELS];
extern bool dumpPins;

#endif