/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef LYNSYN_FW_H
#define LYNSYN_FW_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

#include "efm32gg332f1024.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "em_system.h"
#include "em_chip.h"
 
#ifdef VERSION2
#define SENSORS 7
#else
#define SENSORS 3
#endif

///////////////////////////////////////////////////////////////////////////////
// flash layout

#define FLASH_BOOT_START       0x000000
#define FLASH_BOOT_VERSION     0x000100

#define FLASH_APP_START        0x010000

#define FLASH_UPGRADE_START    0x080000

#define FLASH_PARAMETERS_START 0x0f0000
#define FLASH_UPGRADE_FLAG     0x0f0000

#define UPGRADE_MAGIC 0x4e415050

///////////////////////////////////////////////////////////////////////////////
// lynsyn settings

#define CLOCK_FREQ 48000000

#define LED_ON  0
#define LED_OFF 1

#ifdef VERSION2

#define LED0_BIT 0
#define LED0_PORT gpioPortC

#define PROGRAM_B_BIT 1
#define PROGRAM_B_PORT gpioPortA

#define DONE_BIT 3
#define DONE_PORT gpioPortA

#define FPGA_USART USART0
#define FPGA_USART_CLK cmuClock_USART0
#define FPGA_USART_LOC USART_ROUTE_LOCATION_LOC0

#define FPGA_PORT gpioPortE

#define FPGA_TX_BIT  10
#define FPGA_RX_BIT  11
#define FPGA_CLK_BIT 12
#define FPGA_CS_BIT  13
#define FPGA_INT_BIT 14

#else

#define LED0_BIT 1
#define LED0_PORT gpioPortC

#define JTAG_USART USART0
#define JTAG_USART_CLK cmuClock_USART0
#define JTAG_USART_LOC USART_ROUTE_LOCATION_LOC0

#define TDI_USART USART1
#define TDI_USART_CLK cmuClock_USART1
#define TDI_USART_LOC USART_ROUTE_LOCATION_LOC0

#define JTAG_PORT gpioPortE
#define JTAG_TMS_BIT  10
#define JTAG_TDO_BIT  11
#define JTAG_TCK_BIT  12
#define JTAG_TRST_BIT 13

#define TDI_PORT gpioPortC
#define TDI_BIT 0

#define SWD_USART JTAG_USART
#define SWD_USART_CLK JTAG_USART_CLK
#define SWD_USART_LOC JTAG_USART_LOC

#define SWD_PORT gpioPortE
#define SWD_CLK JTAG_TCK_BIT
#define SWD_IO JTAG_TMS_BIT

#endif

///////////////////////////////////////////////////////////////////////////////
// global functions

void setLed(int led);
void clearLed(int led);

#endif
