/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#include <stdio.h>

#include "lynsyn_fw.h"

void clearLed(int led) {
  switch(led) {
    case 0:
      GPIO_PinOutSet(LED0_PORT, LED0_BIT);
      break;
  }
}

void setLed(int led) {
  switch(led) {
    case 0:
      GPIO_PinOutClear(LED0_PORT, LED0_BIT);
      break;
  }
}

