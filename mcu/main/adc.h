/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef ADC_H
#define ADC_H

#include <em_adc.h>
#include <em_dma.h>

#define ADC_CLOCK 10000000

void adcCalibrate(unsigned sensor, int32_t adcCalValue);
void adcInit(void);
void adcScan(int16_t *sampleBuffer);
void adcScanWait(void);

#endif
