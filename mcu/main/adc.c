/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#include "lynsyn_main.h"
#include "adc.h"
#include "config.h"

#include <em_cmu.h>
#include <dmactrl.h>

#define REFERENCE adcRefExtSingle
#define ADC_DMA_CHANNEL 0

void adcInit(void) {
  CMU_ClockEnable(cmuClock_DMA, true);
  
  DMA_Init_TypeDef dmaInit;
  dmaInit.hprot = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  DMA_CfgChannel_TypeDef chnlCfg;
  chnlCfg.highPri = false;
  chnlCfg.enableInt = false;
  chnlCfg.select = DMAREQ_ADC0_SCAN;
  chnlCfg.cb = NULL;
  DMA_CfgChannel(ADC_DMA_CHANNEL, &chnlCfg);

  DMA_CfgDescr_TypeDef descrCfg;
  descrCfg.dstInc = dmaDataInc2;
  descrCfg.srcInc = dmaDataIncNone;
  descrCfg.size = dmaDataSize2;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot = 0;
  DMA_CfgDescr(ADC_DMA_CHANNEL, true, &descrCfg);
  
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(ADC_CLOCK, 0);
  init.lpfMode = ADC_CTRL_LPFMODE_DECAP;
  init.ovsRateSel = adcOvsRateSel8;
  ADC_Init(ADC0, &init);

  ADC_InitScan_TypeDef scanInit = ADC_INITSCAN_DEFAULT;
  scanInit.reference = REFERENCE;
#ifdef VERSION2
  scanInit.input = ADC_SCANCTRL_INPUTMASK_CH0 |
                   ADC_SCANCTRL_INPUTMASK_CH1 |
                   ADC_SCANCTRL_INPUTMASK_CH2 |
                   ADC_SCANCTRL_INPUTMASK_CH3 |
                   ADC_SCANCTRL_INPUTMASK_CH4 |
                   ADC_SCANCTRL_INPUTMASK_CH5 |
                   ADC_SCANCTRL_INPUTMASK_CH7;
#else
  scanInit.input = ADC_SCANCTRL_INPUTMASK_CH0 |
                   ADC_SCANCTRL_INPUTMASK_CH1 |
                   ADC_SCANCTRL_INPUTMASK_CH2 |
                   ADC_SCANCTRL_INPUTMASK_CH3 |
                   ADC_SCANCTRL_INPUTMASK_CH4 |
                   ADC_SCANCTRL_INPUTMASK_CH5;
#endif
  scanInit.resolution = adcResOVS;
  ADC_InitScan(ADC0, &scanInit);
}

void adcScan(int16_t *sampleBuffer) {
#ifdef VERSION2
  DMA_ActivateBasic(ADC_DMA_CHANNEL,
                    true,
                    false,
                    sampleBuffer,
                    (void *)((uint32_t)&(ADC0->SCANDATA)),
                    6);
#else
  DMA_ActivateBasic(ADC_DMA_CHANNEL,
                    true,
                    false,
                    sampleBuffer,
                    (void *)((uint32_t)&(ADC0->SCANDATA)),
                    5);
#endif
  
  ADC_Start(ADC0, adcStartScan);
}

void adcScanWait(void) {
  while(ADC0->STATUS & ADC_STATUS_SCANACT);
}
