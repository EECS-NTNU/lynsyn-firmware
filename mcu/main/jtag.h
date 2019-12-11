#ifndef JTAG_H
#define JTAG_H

extern uint32_t dpIdcode;

bool jtagInit(struct JtagDevice *devlist);
bool jtagDpLowAccess(uint8_t RnW, uint16_t addr, uint32_t writeValue, uint32_t *readValue);
void jtagWriteIr(uint32_t idcode, uint32_t ir);
bool jtagReadWriteDr(uint32_t idcode, uint8_t *din, uint8_t *dout, int size);

#endif
