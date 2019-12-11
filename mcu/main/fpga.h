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
