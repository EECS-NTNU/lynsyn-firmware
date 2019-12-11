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

#include "lynsyn_main.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <em_device.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_gpio.h>
#include <em_emu.h>

#include "../common/swo.h"
#include "../common/usbprotocol.h"

#include "arm.h"
#include "jtag_lowlevel.h"
#include "adc.h"
#include "config.h"
#include "usb.h"
#include "jtag.h"
#ifdef VERSION2
#include "fpga.h"
#endif

volatile bool sampleMode;
volatile bool samplePc;
volatile bool useStopBp;
volatile int64_t sampleStop;

volatile uint32_t lastLowWord = 0;
volatile uint32_t highWord = 0;

static struct SampleReplyPacket sampleBuf1[MAX_SAMPLES] __attribute__((__aligned__(4)));
static struct SampleReplyPacket sampleBuf2[MAX_SAMPLES] __attribute__((__aligned__(4)));

static int16_t continuousCurrent[CHANNELS];
uint64_t continuousCurrentAcc[CHANNELS];
int continuousSamplesSinceLast[CHANNELS];
int16_t continuousCurrentInstant[CHANNELS];
int16_t continuousCurrentAvg[CHANNELS];

///////////////////////////////////////////////////////////////////////////////

void panic(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf("\nPanic: ");
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  while(true);
}

///////////////////////////////////////////////////////////////////////////////

int main(void) {
  sampleMode = false;

#ifndef __linux__
  // setup clocks
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_USB, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
#endif

  printf("Lynsyn initializing...\n");

#ifndef __linux__
  // Enable cycle counter
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= 0x01;
  DWT->CYCCNT = 0;
#endif

  configInit();

#ifdef __linux__
  printf("Hardware V%x.%x Firmware %s\n", (getUint32("hwver") & 0xf0) >> 4, getUint32("hwver") & 0xf, SW_VERSION_STRING);
#else
  printf("Hardware V%lx.%lx Firmware %s\n", (getUint32("hwver") & 0xf0) >> 4, getUint32("hwver") & 0xf, SW_VERSION_STRING);
#endif

  jtagInitLowLevel();
  adcInit();
  usbInit();

  clearLed(0);

#if 0
  {
    /* DWT->CYCCNT = 0; */
    /* while(DWT->CYCCNT < 480000000); */

    struct JtagDevice devices[SIZE_JTAG_DEVICE_LIST];

    memset(devices, 0, sizeof(devices));

    devices[0].idcode = 0x4ba00477;
    devices[0].irlen = 4;

    devices[1].idcode = 0x1372c093;
    devices[1].irlen = 6;

    devices[2].idcode = 0x5ba00477;
    devices[2].irlen = 4;

    devices[3].idcode = 0x14710093;
    devices[3].irlen = 12;

    devices[4].idcode = 0x04721093;
    devices[4].irlen = 12;

    devices[5].idcode = 0x28e20126;
    devices[5].irlen = 12;

    if(armInitCores(devices)) {

      coreReadPcsrInit(0x1);
  
      for(int i = 0; i < 100; i++) {
        uint64_t pc[4];
        bool halted;

        if(coreReadPcsrFast(pc, &halted)) {
          printf("%d: %llx %llx %llx %llx\n", i, pc[0], pc[1], pc[2], pc[3]);
        }
      }
    }
  }
#endif

  printf("Ready.\n");

  int samples = 0;

  unsigned currentSample = 0;

  struct SampleReplyPacket *sampleBuf = sampleBuf1;

  // main loop
  while(true) {
#ifdef __linux__
    linux_processUsb();
#endif

    if(sampleMode) {
      int64_t currentTime = calculateTime();

      struct SampleReplyPacket *samplePtr = &sampleBuf[currentSample];

      samplePtr->flags = 0;

      bool halted = false;
      bool sampleOk = true;

      {
        samplePtr->time = currentTime;

        adcScan(samplePtr->channel);
        if(samplePc) {
          sampleOk = coreReadPcsrFast(samplePtr->pc, &halted);
          if(!sampleOk) halted = false;
          if(sampleOk && halted) {
            for(int i = 0; i < numCores; i++) {
              uint64_t pc;
              if(!readPc(i, &pc)) {
                printf("Warning, can't read PC\n");
              }
              if(pc == markBp) {
                clearBp(MARK_BP);
                coresResume();
                halted = false;
                samplePtr->flags = SAMPLE_REPLY_FLAG_MARK;
                setBp(MARK_BP, markBp);
                break;
              }
            }
          }
        } else {
          for(int i = 0; i < 4; i++) {
            samplePtr->pc[i] = 0;
          }
        }

        if(!useStopBp) {
          halted = currentTime >= sampleStop;
        }

        adcScanWait();
      }

      if(halted) {
        samplePtr->time = -1;
        sampleMode = false;
        sampleOk = true;

        if(useStopBp) {
          clearBp(STOP_BP);
          coresResume();
        }

#ifdef VERSION2
        jtagExt();
#endif
        clearLed(0);

        printf("Exiting sample mode, %d samples\n", samples);

        samples = 0;
      }

      {
        if(sampleOk) {
          samples++;
          currentSample++;

          if(samplePtr->flags & SAMPLE_REPLY_FLAG_MARK) samplePtr->pc[0] = calculateTime();

          if((currentSample >= MAX_SAMPLES) || halted) {
            sendSamples(sampleBuf, currentSample);
            if(sampleBuf == sampleBuf1) sampleBuf = sampleBuf2;
            else sampleBuf = sampleBuf1;
            currentSample = 0;
          }
        }
      }
    } else {
#ifndef __linux__
      __disable_irq();
#endif

      adcScan(continuousCurrent);
      adcScanWait();

      for(int i = 0; i < CHANNELS; i++) {
        continuousCurrentAcc[i] += continuousCurrent[i];
        continuousSamplesSinceLast[i]++;
        continuousCurrentInstant[i] = continuousCurrent[i];
      }

#ifndef __linux__
      __enable_irq();
#endif
    }
  }
}

void wait(unsigned cycles) {
#ifndef __linux__
  DWT->CYCCNT = 0;
  while(DWT->CYCCNT < cycles);
#endif
}

int64_t calculateTime() {
#ifdef __linux__
  return 0;
#else
  uint32_t lowWord = DWT->CYCCNT;
  __disable_irq();
  if(lowWord < lastLowWord) highWord++;
  lastLowWord = lowWord;
  __enable_irq();
  return ((uint64_t)highWord << 32) | lowWord;
#endif
}

int _write(int fd, char *str, int len) {
#ifndef __linux__
  for (int i = 0; i < len; i++) {
    ITM_SendChar(str[i]);
  }
#endif
  return len;
}

void getCurrentAvg(int16_t *sampleBuffer) {
  for(int i = 0; i < CHANNELS; i++) {
    sampleBuffer[i] = continuousCurrentAcc[i] / continuousSamplesSinceLast[i];
    continuousCurrentAcc[i] = 0;
    continuousSamplesSinceLast[i] = 0;
  }
}
