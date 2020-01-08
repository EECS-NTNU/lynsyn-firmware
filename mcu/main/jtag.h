/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
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

#ifndef JTAG_H
#define JTAG_H

extern uint32_t dpIdcode;

bool jtagInit(struct JtagDevice *devlist);
bool jtagDpLowAccess(uint8_t RnW, uint16_t addr, uint32_t writeValue, uint32_t *readValue);
void jtagWriteIr(uint32_t idcode, uint32_t ir);
bool jtagReadWriteDr(uint32_t idcode, uint8_t *din, uint8_t *dout, int size);

#endif
