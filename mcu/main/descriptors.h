/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include "em_usb.h"

extern const USB_DeviceDescriptor_TypeDef USBDESC_deviceDesc;
extern const uint8_t                      USBDESC_configDesc[];
extern const void * const                 USBDESC_strings[3];
extern const uint8_t                      USBDESC_bufferingMultiplier[];

#endif
