/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef JTAG_LOWLEVEL_H
#define JTAG_LOWLEVEL_H

#include "../common/usbprotocol.h"

#include <stdbool.h>

void readWriteSeq(unsigned size, uint8_t *tdiData, uint8_t *tmsData, uint8_t *tdoData);
void storeSeq(uint16_t size, uint8_t *tdiData, uint8_t *tmsData);
void storeProg(unsigned size, uint8_t *read, uint16_t *initPos, uint16_t *loopPos, uint16_t *ackPos, uint16_t *endPos);
bool executeSeq(void);
uint32_t *getWords(void);

uint32_t jtagSetBaudrate(uint32_t baudrate);
bool jtagInitLowLevel(void);
void setTrst(bool on);

#endif
