/******************************************************************************
 *
 *  This file is part of the TULIPP Lynsyn Power Measurement Utilitity
 *
 *  Copyright 2018 Asbjørn Djupdal, NTNU, TULIPP EU Project
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
// Initialization, test and calibration program for Lynsyn V3.0 (Lynsyn Light)
//
///////////////////////////////////////////////////////////////////////////////

#include <argp.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lynsyn.h"

#define SENSORS 3

///////////////////////////////////////////////////////////////////////////////

double rsDefault[SENSORS] = {0.025, 0.05, 0.1};
unsigned hwVersion = HW_VERSION_3_0;

///////////////////////////////////////////////////////////////////////////////

bool programMcu(char *bootFilename, char *mainFilename) {
  printf("Flashing %s and %s\n", bootFilename, mainFilename);

  char command[256];
  snprintf(command, 256, "commander flash %s --halt --device EFM32GG332F1024", bootFilename);

  int ret = system(command);
  if(!ret) {
    snprintf(command, 256, "commander flash %s --address 0x10000 --device EFM32GG332F1024", mainFilename);
    ret = system(command);
  }

  if(ret) {
    printf("Can't program the MCU.  Possible problems:\n");
    printf("- SiLabs Simplicity Commander problem\n");
    printf("- Lynsyn USB port is not connected to the PC\n");
    printf("- EFM32 programmer is not connected to both PC and Lynsyn\n");
    printf("- Faulty soldering or components:\n");
    printf("  - U1 MCU\n");
    printf("  - J5 Cortex debug\n");
    return false;
  }

  printf("MCU flashed OK\n");
  return true;
}

///////////////////////////////////////////////////////////////////////////////

bool cleanNonVolatile(void) {
  double rs[LYNSYN_MAX_SENSORS];
  char text[80];

  for(int i = 0; i < SENSORS; i++) {
    printf("*** Enter Rs%d [%f ohm]: ", i+1, rsDefault[i]);
    if(!fgets(text, 80, stdin)) {
      printf("I/O error\n");
      exit(-1);
    }
    if(text[0] == '\n') rs[i] = rsDefault[i];
    else rs[i] = strtod(text, NULL);
  }

  printf("Initializing HW %x with Rs %f %f %f\n", hwVersion, rs[0], rs[1], rs[2]);
  return lynsyn_cleanNonVolatile(hwVersion, rs);
}

///////////////////////////////////////////////////////////////////////////////

bool testUsb(void) {
  if(!lynsyn_testUsb()) {
    printf("Can't communicate over USB.  Possible problems:\n");
    printf("- Problems with libusb\n");
    printf("- Lynsyn USB port is not connected to the PC\n");
    printf("- Faulty soldering or components:\n");
    printf("  - U1 MCU\n");
    printf("  - J1 USB\n");
    printf("  - Y1 48MHz crystal\n");
    return false;
  }

  printf("USB communication OK\n");

  return true;
}

///////////////////////////////////////////////////////////////////////////////

void calibrateSensorCurrent(int sensor, double acceptance) {
  if((sensor < 0) || (sensor > 2)) {
    printf("Incorrect sensor number: %d\n", sensor+1);
    exit(-1);
  }

  uint8_t hwVersion, bootVersion, swVersion;
  double r[LYNSYN_MAX_SENSORS];
  lynsyn_getInfo(&hwVersion, &bootVersion, &swVersion, r);

  printf("Calibrate current sensor %d\n", sensor + 1);

  { // current
    char calCurrent[80];

    double shuntSizeVal = r[sensor];
    double maxCurrentVal = lynsyn_getMaxCurrent(shuntSizeVal);

    printf("%f ohm shunt resistor gives a maximum current of %fA\n\n", shuntSizeVal, maxCurrentVal);

    double currents[2];
    currents[0] = maxCurrentVal*0.04;
    currents[1] = maxCurrentVal*0.96;
    
    double calCurrentVal;

    for(int point = 0; point < sizeof(currents)/sizeof(double); point++) {

      printf("*** Connect a calibration current source.\n"
             "The current should be around %fA.\n"
             "Do not trust the current source display, use a multimeter to confirm.\n"
             "Enter measured current:\n", currents[point]);
      if(!fgets(calCurrent, 80, stdin)) {
        printf("I/O error\n");
        exit(-1);
      }

      calCurrentVal = strtod(calCurrent, NULL);

      printf("Calibrating sensor %d with low calibration current %f\n\n", sensor+1, calCurrentVal);

      lynsyn_adcCalibrateCurrent(sensor, calCurrentVal, maxCurrentVal);
    }

    ///////////////////////////////////////////////////////////////////////////////

    if(!lynsyn_testAdcCurrent(sensor, calCurrentVal, acceptance)) {
      printf("Calibration error\n");
      exit(-1);
    }

    printf("\n");
  }
}

void calibrateSensorVoltage(int sensor, double acceptance) {
  if((sensor < 0) || (sensor > 2)) {
    printf("Incorrect sensor number: %d\n", sensor+1);
    exit(-1);
  }

  uint8_t hwVersion, bootVersion, swVersion;
  double r[LYNSYN_MAX_SENSORS];
  lynsyn_getInfo(&hwVersion, &bootVersion, &swVersion, r);

  { // voltage
    char calVoltage[80];

    double maxVoltageVal = lynsyn_getMaxVoltage();

    printf("Calibrate voltage sensor %d\n", sensor + 1);

    ///////////////////////////////////////////////////////////////////////////////

    double voltages[3] = {1, 11, 22};
    double calVoltageVal;

    for(int point = 0; point < sizeof(voltages)/sizeof(double); point++) {

      printf("*** Connect a calibration voltage source.\n"
             "The voltage should be around %fV.\n"
             "Do not trust the voltage source display, use a multimeter to confirm.\n"
             "Enter measured voltage:\n", voltages[point]);
      if(!fgets(calVoltage, 80, stdin)) {
        printf("I/O error\n");
        exit(-1);
      }

      calVoltageVal = strtod(calVoltage, NULL);

      printf("Calibrating sensor %d with low calibration voltage %f\n\n", sensor+1, calVoltageVal);

      lynsyn_adcCalibrateVoltage(sensor, calVoltageVal, maxVoltageVal);
    }

    ///////////////////////////////////////////////////////////////////////////////

    if(!lynsyn_testAdcVoltage(sensor, calVoltageVal, acceptance)) {
      printf("Calibration error\n");
      exit(-1);
    }

    printf("\n");
  }
}

///////////////////////////////////////////////////////////////////////////////

void programTest(void) {
  printf("First step: Manual tests.\n\n");

  {
    printf("*** Connect Lynsyn to the PC USB port.\n");
    getchar();

    printf("*** Measure the voltage across C18.  Should be 3.3V.\n");
    getchar();

    printf("*** Verify that LED D1 is lit.\n");
    getchar();
  }

  {
    printf("*** Connect a multimeter to TP1.  Adjust RV1 until the voltage gets as close to 2.5V as possible.\n");
    getchar();
  }

  printf("*** Secure RV1 by applying a drop of nail polish on top.\n");
  getchar();

  printf("Second step: Flashing.\n\n");

  printf("*** Connect the EFM32 starter kit to Lynsyn J5 (Cortex Debug).\n");
  getchar();

  printf("*** Enter boot bin filename [fwbin/lynsyn_boot.bin]:\n");
  char bootFilename[80];
  if(!fgets(bootFilename, 80, stdin)) {
    printf("I/O error\n");
    exit(-1);
  }
  if(bootFilename[0] == '\n') strncpy(bootFilename, "fwbin/lynsyn_boot.bin", 80);
  bootFilename[strcspn(bootFilename, "\n")] = 0;

  printf("*** Enter main bin filename [fwbin/lynsyn_boot.bin]:\n");
  char mainFilename[80];
  if(!fgets(mainFilename, 80, stdin)) {
    printf("I/O error\n");
    exit(-1);
  }
  if(mainFilename[0] == '\n') strncpy(mainFilename, "fwbin/lynsyn_main.bin", 80);
  mainFilename[strcspn(mainFilename, "\n")] = 0;

  if(!programMcu(bootFilename, mainFilename)) {
    printf("Can't program lynsyn\n");
    exit(-1);
  }

  printf("\nThird step: Automatic tests.\n\n");

  if(!lynsyn_preinit()) {
    printf("Can't initialize lynsyn\n");
    exit(-1);
  }

  if(!lynsyn_testUsb()) {
    printf("USB failure\n");
    exit(1);
  }

  printf("\nFourth step: Initializing non-volatile memory.\n\n");

  if(!cleanNonVolatile()) exit(-1);

  printf("\nFift step: More manual tests.\n\n");

  lynsyn_setLed(true);

  printf("*** Verify that LED D2 is lit.\n");
  getchar();

  lynsyn_setLed(false);

  printf("*** Verify that LED D2 is unlit.\n");
  getchar();

  lynsyn_prerelease();
  if(!lynsyn_init()) {
    printf("Can't init lynsyn.\n");
    exit(-1);
  }
}

void programTestAndCalibrate(double acceptance) {
  programTest();

  printf("Sixt step: Current sensor calibration.\n\n");

  for(int i = 0; i < SENSORS; i++) {
    calibrateSensorCurrent(i, acceptance);
  }

  printf("Seventh step: Voltage sensor calibration.\n\n");
  printf("This lynsyn has a maximum voltage of %fV\n\n", lynsyn_getMaxVoltage());

  for(int i = 0; i < SENSORS; i++) {
    calibrateSensorVoltage(i, acceptance);
  }

  lynsyn_release();

  printf("\nAll tests OK and all calibrations done.\n");
}

void calCurrentSensor(double acceptance) {
  if(!lynsyn_init()) {
    printf("Can't initialize lynsyn\n");
    exit(-1);
  }

  char sensor[80] = "";

  while(sensor[0] != 'x') {
    printf("Which sensor do you want to calibrate ('x' for exit)?\n");
    if(!fgets(sensor, 80, stdin)) {
      printf("I/O error\n");
      exit(-1);
    }
    if((sensor[0] != 'x') && (sensor[0] != 'X'))  {
      int s = strtol(sensor, NULL, 10)-1;
      calibrateSensorCurrent(s, acceptance);
    }
  }

  lynsyn_release();
}

void calVoltageSensor(double acceptance) {
  if(!lynsyn_init()) {
    printf("Can't initialize lynsyn\n");
    exit(-1);
  }

  char sensor[80] = "";

  while(sensor[0] != 'x') {
    printf("Which sensor do you want to calibrate ('x' for exit)?\n");
    if(!fgets(sensor, 80, stdin)) {
      printf("I/O error\n");
      exit(-1);
    }
    if((sensor[0] != 'x') && (sensor[0] != 'X'))  {
      int s = strtol(sensor, NULL, 10)-1;
      calibrateSensorVoltage(s, acceptance);
    }
  }

  lynsyn_release();
}

bool live(void) {
  if(!lynsyn_init()) {
    printf("Can't initialize lynsyn\n");
    exit(-1);
  }

  while(true) {
    struct LynsynSample sample;

    lynsyn_getSample(&sample, true, 0);

    for(int i = 0; i < LYNSYN_MAX_CORES; i++) {
      printf("%lx ", sample.pc[i]);
    }
    printf(" : ");
    for(int i = 0; i < LYNSYN_MAX_SENSORS; i++) {
      printf("%f/%f ", sample.current[i], sample.voltage[i]);
    }
    printf("\n");

    sleep(1);
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////

static char doc[] = "A test and calibration tool for Lynsyn boards";
static char args_doc[] = "";

static struct argp_option options[] = {
  {"board-version", 'b', "version",   0, "Board version" },
  {"procedure",     'p', "procedure", 0, "Which procedure to run" },
  {"acceptance",    'a', "value",     0, "Maximum allowed error in percentage (0.01 default" },
  { 0 }
};

struct arguments {
  int procedure;
  double acceptance;
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key) {
    case 'p':
      arguments->procedure = strtol(arg, NULL, 10);
      break;
    case 'a':
      arguments->acceptance = strtod(arg, NULL);
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 0)
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char *argv[]) {
  struct arguments arguments;
  arguments.procedure = -1;
  arguments.acceptance = 0.01;

  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  if(arguments.procedure < 1) { 
    char choiceBuf[80];

    printf("Which procedure do you want to perform?\n");
    printf("Enter '1' for complete programming, test and calibration.\n");
    printf("Enter '2' for only current sensor calibration\n");
    printf("Enter '3' for only voltage sensor calibration\n");
    printf("Enter '4' for live measurements\n");
    printf("Enter '5' for USB firmware upgrade\n");
    printf("Enter '6' for complete programming and testing, excluding calibration.\n");
    if(!fgets(choiceBuf, 80, stdin)) {
      printf("I/O error\n");
      exit(-1);
    }

    arguments.procedure = strtol(choiceBuf, NULL, 10);
  }
    
  printf("\n");

  switch(arguments.procedure) {
    case 1:
      printf("This procedure programs, tests and calibrates the Lynsyn board.\n"
             "All lines starting with '***' requires you to do a certain action, and then press enter to continue.\n\n");

      programTestAndCalibrate(arguments.acceptance);
      break;

    case 2:
      printf("*** Connect Lynsyn to the PC USB port.\n");
      getchar();

      calCurrentSensor(arguments.acceptance);
      break;

    case 3:
      printf("*** Connect Lynsyn to the PC USB port.\n");
      getchar();

      calVoltageSensor(arguments.acceptance);
      break;

    case 4:
      printf("*** Connect Lynsyn to the PC USB port.\n");
      getchar();

      live();
      break;

    case 5:
      printf("*** Connect Lynsyn to the PC USB port.\n");
      getchar();

      //lynsyn_usbFirmwareUpgrade();
      break;

    case 6:
      printf("This procedure programs and tests the Lynsyn board.\n"
             "All lines starting with '***' requires you to do a certain action, and then press enter to continue.\n\n");

      programTest();
      break;

    default:
      return 0;
  }

  return 0;
}

