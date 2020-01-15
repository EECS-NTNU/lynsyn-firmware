/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef LYNSYN_BOOT_H
#define LYNSYN_BOOT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#include "../common/lynsyn_fw.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "em_system.h"
#include "em_chip.h"
 
#define USE_SWO

#define BOOT_VERSION 0x11
#define BOOT_VERSION_STRING "V1.1"

#endif
