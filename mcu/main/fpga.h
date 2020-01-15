/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef FPGA_H
#define FPGA_H

#ifdef VERSION2

#include "../common/usbprotocol.h"

#define FPGA_JTAG_CLOCK 10000000

#define FPGA_CONFIGURE_TIMEOUT 96000000 // in cycles

#define MAX_SEQ_SIZE 256
#define MAX_STORED_SEQ_SIZE 65536

void jtagInt(void);
void jtagExt(void);

#endif
#endif
