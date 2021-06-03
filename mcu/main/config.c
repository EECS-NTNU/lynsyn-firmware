/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#include "lynsyn_main.h"
#include "config.h"

#include <em_msc.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define CONFIG_LINES (CONFIG_FLASH_SIZE/sizeof(struct configLine))

struct configLine {
  char id[ID_SIZE];
  union {
    char value[VALUE_SIZE];
    uint32_t uint32Value;
    int16_t int16Value;
    double doubleValue;
  };
};

struct configLine configMem[CONFIG_LINES+1];

static void flushConfig(void) {
  MSC_Init();
  MSC_ErasePage((uint32_t*)CONFIG_FLASH_ADDRESS);
  if(MSC_WriteWord((uint32_t*)CONFIG_FLASH_ADDRESS, configMem, CONFIG_FLASH_SIZE) != mscReturnOk) {
    printf("Can't write flash\n");
  }
  MSC_Deinit();
}

void configInit(void) {
  memcpy(configMem, (uint32_t*)CONFIG_FLASH_ADDRESS, CONFIG_FLASH_SIZE);

  if(strncmp(configMem[0].id, CONFIG_FLASH_ID, strlen(CONFIG_FLASH_ID))) {
    printf("Initializing flash\n");
    strcpy(configMem[0].id, CONFIG_FLASH_ID);
    strcpy(configMem[0].value, CONFIG_FLASH_VERSION);

    for(int i = 1; i < CONFIG_LINES; i++) {
      configMem[i].id[0] = 0;
      configMem[i].value[0] = 0;
    }
    flushConfig();
  }
}

void printConfigMem(void) {
  printf("Config MEM:\n");
  for(int i = 1; i < CONFIG_LINES; i++) {
    if(configMem[i].id[0]) {
      printf("  %s\n", configMem[i].id);
    } else {
      printf("  <empty>\n");
    }
  }
}

static struct configLine *getLine(char *id) {
  for(int i = 1; i < CONFIG_LINES; i++) {
    if(!strncmp(configMem[i].id, id, 8)) {
      return &configMem[i];
    }
  }
  return NULL;
}

static struct configLine *getFreeLine(void) {
  for(int i = 1; i < CONFIG_LINES; i++) {
    if(!configMem[i].id[0]) {
      return &configMem[i];
    }
  }
  return NULL;
}

bool configExists(char *id) {
  return getLine(id);
}

int16_t getInt16(char *id) {
  struct configLine *line = getLine(id);
  if(line) {
    return line->int16Value;
  }
  return 0;
}

uint32_t getUint32(char *id) {
  struct configLine *line = getLine(id);
  if(line) {
    return line->uint32Value;
  }
  return 0;
}

double getDouble(char *id) {
  struct configLine *line = getLine(id);
  if(line) {
    return line->doubleValue;
  }
  return 0;
}

char *getString(char *id) {
  struct configLine *line = getLine(id);
  if(line) return line->value;
  return 0;
}

void setInt16(char *id, int16_t val) {
  if(strlen(id) > ID_SIZE) panic("Config id length");

  struct configLine *line = getLine(id);

  if(!line) line = getFreeLine();

  if(line) {
    strcpy(line->id, id);
    line->int16Value = val;
    flushConfig();
  }
}

void setUint32(char *id, uint32_t val) {
  if(strlen(id) > ID_SIZE) panic("Config id length");

  struct configLine *line = getLine(id);

  if(!line) line = getFreeLine();

  if(line) {
    strcpy(line->id, id);
    line->uint32Value = val;
    flushConfig();
  }
}

void setDouble(char *id, double val) {
  if(strlen(id) > ID_SIZE) panic("Config id length");
  
  struct configLine *line = getLine(id);

  if(!line) line = getFreeLine();

  if(line) {
    strcpy(line->id, id);
    line->doubleValue = val;
    flushConfig();
  }
}

void setString(char *id, char *s) {
  if(strlen(id) > ID_SIZE) panic("Config id length");
  if(strlen(s) >= VALUE_SIZE) panic("Config value length");

  struct configLine *line = getLine(id);

  if(!line) line = getFreeLine();

  if(line) {
    strcpy(line->id, id);
    strcpy(line->value, s);
    flushConfig();
  }
}

void clearConfig(void) {
  configMem[0].id[0] = 0;
  flushConfig();
  configInit();
}
