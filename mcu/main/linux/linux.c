#include <stdlib.h>

#include <em_usb.h>

#include "lynsyn_main.h"
#include "adc.h"

#include "jtaginput.c"

extern uint8_t inBuffer[];
int UsbDataReceived(USB_Status_TypeDef status, uint32_t xf, uint32_t remaining);

void clearLed(int led) {
  printf("LED off\n");
}

void setLed(int led) {
  printf("LED on\n");
}

void adcCalibrate(unsigned sensor, int32_t adcCalValue) {
}

void adcInit(void) {
}

void adcScan(int16_t *sampleBuffer) {
  for(int i = 0; i < CHANNELS; i++) {
    sampleBuffer[i] = 0;
  }
}

void adcScanWait(void) {
}

void configInit(void) {
}

void clearConfig(void) {
}

bool configExists(char *id) {
  return false;
}

int16_t getInt16(char *id) {
  return 0;
}

uint32_t getUint32(char *id) {
  if(strcmp(id, "hwver") == 0) {
    return 0x30;
  }

  return 0;
}

double getDouble(char *id) {
  if(strncmp(id, "offset", 6) == 0) {
    return 0;
  }
  if(strncmp(id, "gain", 4) == 0) {
    return 1;
  }
  if(strncmp(id, "r", 1) == 0) {
    return 1;
  }

  return 0;
}

char *getString(char *id) {
  return "";
}

void setInt16(char *id, uint32_t val) {
  printf("Setting %s to %d\n", id, val);
}

void setUint32(char *id, uint32_t val) {
  printf("Setting %s to %d\n", id, val);
}

void setDouble(char *id, double val) {
  printf("Setting %s to %f\n", id, val);
}

void setString(char *id, char *s) {
  printf("Setting %s to %s\n", id, s);
}

void linux_usbWrite(uint8_t *buf, int size) {
  static int state = 0;

  switch(state) {
    case 0: {
      struct InitReplyPacket *initReply = (struct InitReplyPacket*)buf;
      printf("*** HW %x Boot %x SW %x\n", initReply->hwVersion, initReply->bootVersion, initReply->swVersion);
      state++;
      break;
    }

    case 1: {
      // cal data from init
      state++;
      break;
    }

    case 2: {
      struct JtagInitReplyPacket *reply = (struct JtagInitReplyPacket*)buf;
      printf("*** Init %s %d cores\n", reply->success ? "ok" : "failed", reply->numCores);

      state++;
      break;
    }

    case 3: {
      struct SampleReplyPacket *sampleReply = (struct SampleReplyPacket*)buf;
      for(int i = 0; i < MAX_SAMPLES; i++) {
        if(sampleReply->time == -1) break;
        printf("*** Sample %lx %lx %lx %lx\n", sampleReply->pc[0], sampleReply->pc[1], sampleReply->pc[2], sampleReply->pc[3]);
        sampleReply++;
      }
    }
  }
}

void linux_processUsb(void) {
  static int state = 0;

  switch(state) {
    case 0: {
      printf("\n*** Sending init\n");
      struct RequestPacket *initRequest = (struct RequestPacket*)inBuffer;
      initRequest->cmd = USB_CMD_INIT;
      UsbDataReceived(USB_STATUS_OK, 1, 0);

      break;
    }
    case 1: {
      printf("\n*** Sending jtag init\n");

      struct JtagDevice devices[6];

      memset(devices, 0, sizeof(devices));

      devices[0].idcode = 0x4ba00477;
      devices[0].irlen = 4;

      devices[1].idcode = 0x1372c093;
      devices[1].irlen = 6;

      devices[2].idcode = 0x5ba00477;
      devices[2].irlen = 4;

      devices[3].idcode = 0x14710093;
      devices[3].irlen = 12;

      devices[4].idcode = 0x04721093;
      devices[4].irlen = 12;

      devices[5].idcode = 0x28e20126;
      devices[5].irlen = 12;

      struct JtagInitRequestPacket *req = (struct JtagInitRequestPacket*)inBuffer;
      req->request.cmd = USB_CMD_JTAG_INIT;

      memset(req->devices, 0, sizeof(req->devices));
      memcpy(req->devices, devices, 6 * sizeof(struct JtagDevice));

      UsbDataReceived(USB_STATUS_OK, 1, 0);

      break;
    }
    case 2: {
      printf("\n*** Sending start sampling\n");

      struct StartSamplingRequestPacket *req = (struct StartSamplingRequestPacket*)inBuffer;
      req->request.cmd = USB_CMD_START_SAMPLING;
      req->samplePeriod = 480000000;
      req->flags = SAMPLING_FLAG_PERIOD;
      req->cores = 0xf;

      UsbDataReceived(USB_STATUS_OK, 1, 0);

      break;
    }
  }

  state++;
}

unsigned linux_jtagReadBit(void) {
  static int currentBit = 0;
  if(currentBit >= sizeof(jtagInput)/sizeof(unsigned)) {
    printf("No more JTAG data\n");
    exit(-1);
  }
  return jtagInput[currentBit++];
}
