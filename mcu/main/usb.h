/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef USB_H
#define USB_H

#include "lynsyn_main.h"
#include "../common/usbprotocol.h"

#define START_BP 0
#define STOP_BP  1
#define MARK_BP  0

void usbInit(void);
bool sendSamples(struct SampleReplyPacket *sample, unsigned n);

extern struct LogReplyPacket logReply __attribute__((__aligned__(4)));

#endif
