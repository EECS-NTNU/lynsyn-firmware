/******************************************************************************
 *
 * liblynsyn
 *
 * A library for controlling the lynsyn measurement boards
 * Link with libusb-1.0 when using this library
 *
 * 2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef LYNSYN_H
#define LYNSYN_H

#include <stdbool.h>
#include <stdint.h>
#include <usbprotocol.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LYNSYN_FREQ 48000000

#define LYNSYN_MAX_CORES MAX_CORES
#define LYNSYN_MAX_SENSORS MAX_SENSORS

#define SAMPLE_FLAG_MARK       SAMPLE_REPLY_FLAG_MARK    // the sample is a mark
#define SAMPLE_FLAG_HALTED     SAMPLE_REPLY_FLAG_HALTED  // sampling has stopped

struct LynsynJtagDevice {
  uint32_t idcode; /** idcode of device */
  uint32_t irlen; /** irlen of device */
};

struct LynsynSample {
  int64_t time; /** Sample time (in cycles).  Use lynsyn_cyclesToSeconds() to convert to seconds */
  uint64_t pc[LYNSYN_MAX_CORES]; /** Program counter of sampled cores */
  double current[LYNSYN_MAX_SENSORS]; /** Current for all power sensors */
  double voltage[LYNSYN_MAX_SENSORS]; /** Voltage for all power sensors (if available) */
  uint16_t flags; /** bitmask of the SAMPLE_FLAG_* defines */
};

/*****************************************************************************/
/* Initialization and release */

/**
 * Must be called before using other library functions (except for lynsyn_firmwareUpgrade())
 * @return success
 */
bool lynsyn_init(void);

/** Releases all resources */
void lynsyn_release(void);

/**
 * Get information about the connected lynsyn board
 * @return success
*/
bool lynsyn_getInfo(uint8_t *hwVersion, uint8_t *bootVersion, uint8_t *swVersion, double *r);

/*****************************************************************************/
/* Generic JTAG */

/**
 * Initialise JTAG chain.
 *
 * An array of jtag devices must be provided.  This can e.g. be
 * constructed using the lynsyn_getDefaultJtagDevices() function.  The
 * array must include all devices that are on the jtag chain, but the
 * order is not important and there is no harm in including more
 * devices.
 *
 * Must be called before using any JTAG functionality
 *
 * @return success
 */
bool lynsyn_jtagInit(struct LynsynJtagDevice *devices);

/** @return JtagDevice array constructed from given filename */
struct LynsynJtagDevice *lynsyn_getJtagDevices(char *filename);

/** @return JtagDevice array read from default files */
struct LynsynJtagDevice *lynsyn_getDefaultJtagDevices(void);

/**
 * Set JTAG clock frequency
 * @return Resulting clock frequency (may be different from the one requested)
 */
uint32_t lynsyn_setTck(uint32_t period);

/**
 * Set or unset the JTAG TRST pin
 * @return success
 */
bool lynsyn_trst(uint8_t val);

/**
 * Send the given tms and tdi vectors to the JTAG port, returns with the tdoVector filled in
 * @return success
 */
bool lynsyn_shift(int numBits, uint8_t *tmsVector, uint8_t *tdiVector, uint8_t *tdoVector);

/*****************************************************************************/
/* Sampling */

/**
 * Set mark breakpoint
 * @return success
 */
bool lynsyn_setMarkBreakpoint(uint64_t addr);

/**
 * Start period sampling.  This starts sampling immediately and runs for the given amount of time
 * Use lynsyn_getNextSample() to collect the samples. 
 * @param duration Duration in seconds
 * @param cores A bitmask of the cores where PC sampling is performed.  0 means PC sampling is disabled
 */
void lynsyn_startPeriodSampling(double duration, uint64_t cores);

/**
 * Start breakpoint sampling.  Sampling starts when reaching the start breakpoint and stops when reaching the end breakpoint
 * Use lynsyn_getNextSample() to collect the samples. 
 * @param startAddr Address of start breakpoint
 * @param endAddr Address of end breakpoint
 * @param cores A bitmask of the cores where PC sampling is performed.  0 means PC sampling is disabled
 */
void lynsyn_startBpSampling(uint64_t startAddr, uint64_t endAddr, uint64_t cores);

/**
 * Waits for the next sample to be available, and then return with the new sample in *sample
 * Sampling must have been started previously with either lynsyn_startPeriodSampling() or lynsyn_startBpSampling().
 * Sampling has stopped when the returned sample has flag SAMPLE_FLAG_HALTED set
 * @param sample Where the sample is stored
 * @return success
 */
bool lynsyn_getNextSample(struct LynsynSample *sample);

/**
 * Perform a single sample.  Do not use while doing continuous sampling
 * @param sample Where the sample is stored
 * @param average Specifies if the sample is to be an instantaneous value or an average since the previous call.
 * @param cores A bitmask of the cores where PC sampling is performed.  0 means PC sampling is disabled
 * @return success
 */
bool lynsyn_getSample(struct LynsynSample *sample, bool average, uint64_t cores);

/*****************************************************************************/
/* HW setup and calibration */

/**
 * Perform firmware upgrade
 * @param size Size of firmware buffer
 * @param buf Buffer with the firmware data to use
 * @return success
 */
bool lynsyn_firmwareUpgrade(int size, uint8_t *buf);

/**
 * Erases and initialises the non volatile memory
 * @param hwVersion PCB version
 * @param r Array of the shunt resistors for all sensors
 * @return success
 */
bool lynsyn_cleanNonVolatile(uint8_t hwVersion, double *r);

/** Calibrate current sensor */
void lynsyn_adcCalibrateCurrent(uint8_t sensor, double current, double maxCurrent, bool high);

/** Calibrate voltage sensor */
void lynsyn_adcCalibrateVoltage(uint8_t sensor, double current, double maxCurrent, bool high);

/** Set calibration value */
bool lynsyn_calSet(uint8_t sensor, double offset, double gain);

/** Set calibration value */
bool lynsyn_calGet(uint8_t sensor, double *offsetCurrent, double *gainCurrent, double *offsetVoltage, double *gainVoltage);

/*****************************************************************************/
/* Tests */

bool lynsyn_testUsb(void);
bool lynsyn_testAdcCurrent(unsigned sensor, double val, double acceptance);
bool lynsyn_testAdcVoltage(unsigned sensor, double val, double acceptance);
void lynsyn_setLed(bool on);

/*****************************************************************************/
/* Other */

// Convert lynsyn cycles to seconds
#define lynsyn_cyclesToSeconds(X) ((X) / (double)LYNSYN_FREQ)

// Convert seconds to lynsyn cycles
#define lynsyn_secondsToCycles(X) ((uint64_t)((X) * LYNSYN_FREQ))

/** perform CRC32 the same way as done in the lynsyn firmware */
uint32_t lynsyn_crc32(uint32_t crc, uint32_t *data, int length);

/** @return Maximum current with the given shunt resistor */
double lynsyn_getMaxCurrent(double rl);

/** @return Maximum voltage */
double lynsyn_getMaxVoltage(void);

/** @return Number of cores in the JTAG chain */
unsigned lynsyn_numCores(void);

/** @return Number of power sensors on the connected lynsyn board */
unsigned lynsyn_numSensors(void);

/*****************************************************************************/
/* Internal, do not use */

bool lynsyn_preinit(void);
void lynsyn_prerelease(void);

#ifdef __cplusplus
}
#endif

#endif