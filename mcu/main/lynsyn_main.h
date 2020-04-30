/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
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
void clearLog(void);
void addLogLine(const char *fmt, ...);

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
