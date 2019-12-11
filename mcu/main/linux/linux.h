#ifndef LINUX_H
#define LINUX_H

void linux_processUsb(void);
void linux_usbWrite(uint8_t *buf, int size);
unsigned linux_jtagReadBit(void);

#endif
