#include <argp.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <string.h>
#include <iostream>
#include <iomanip>

#include <lynsyn.h>

struct TraceHeader {
  uint32_t magic;
  uint32_t stream_id;
};

struct TraceEvent {
  uint32_t id;
  uint64_t timestamp;

  uint64_t pc[4];
  double current[3];
  double voltage[3];
};

static char doc[] = "A sampling tool for Lynsyn boards";
static char args_doc[] = "";

static struct argp_option options[] = {
  {"cores",     'c', "cores",     0, "Cores to sample" },
  {"startaddr", 's', "startaddr", 0, "Start Address" },
  {"endaddr",   'e', "endaddr",   0, "End Address" },
  {"frameaddr", 'f', "frameaddr", 0, "Frame Address" },
  {"duration",  'd', "duration",  0, "Duration" },
  {"output",    'o', "filename",  0, "Output File" },
  { 0 }
};

struct arguments {
  uint64_t cores;
  bool useBp;
  bool useFrameBp;
  uint64_t startAddr;
  uint64_t endAddr;
  uint64_t frameAddr;
  double duration;
  std::string output;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments*)state->input;

  switch(key) {
    case 'c':
      arguments->cores = strtol(arg, NULL, 0);
      break;
    case 'b':
      arguments->useBp = true;
      break;
    case 's':
      arguments->startAddr = strtol(arg, NULL, 0);
      arguments->useBp = true;
      break;
    case 'e':
      arguments->endAddr = strtol(arg, NULL, 0);
      break;
    case 'f':
      arguments->frameAddr = strtol(arg, NULL, 0);
      arguments->useFrameBp = true;
      break;
    case 'd':
      arguments->duration = strtol(arg, NULL, 0);
      break;
    case 'o':
      arguments->output = arg;
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
  arguments.cores = 0;
  arguments.useBp = false;
  arguments.useFrameBp = false;
  arguments.startAddr = 0;
  arguments.endAddr = 0;
  arguments.frameAddr = 0;
  arguments.duration = 10;
  arguments.output = "output.csv";

  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  if(arguments.useBp) {
    printf("Sampling PC and power\n");
    printf("From breakpoing %lx to breakpoint %lx", arguments.startAddr, arguments.endAddr);
    if(arguments.useFrameBp) printf(" using frame breakpoint %lx\n", arguments.frameAddr);
    else printf("\n");
    printf("Maximum duration %fs\n", arguments.duration);
    printf("Output file: %s\n", arguments.output.c_str());

  } else {
    printf("Sampling power\n");
    printf("Duration %fs\n", arguments.duration);
    printf("Output file: %s\n", arguments.output.c_str());
  }

  if(lynsyn_init()) {
    if(arguments.useBp || arguments.cores) {
      if(!lynsyn_jtagInit(lynsyn_getDefaultJtagDevices())) {
        printf("Can't init JTAG chain\n");
        exit(-1);
      }
    }

    if(arguments.useBp) {
      lynsyn_startBpSampling(arguments.startAddr, arguments.endAddr, arguments.cores);
    } else {
      lynsyn_startPeriodSampling(arguments.duration, arguments.cores);
    }

    unsigned cores = 0;
    for(int i = 0; i < 8; i++) {
      if(arguments.cores & (1 << i)) cores++;
    }

    std::ofstream file(arguments.output);
    if(!file.fail()) {

      file << "Sensors;Cores\n";
      file << lynsyn_numSensors() << ";" << cores << "\n";

      file << "Time" << std::setprecision(9) << std::fixed;
      for(int i = 0; i < MAX_CORES; i++) {
        file << ";pc " << i;
      }
      for(int i = 0; i < LYNSYN_MAX_SENSORS; i++) {
        file << ";current " << i;
      }
      for(int i = 0; i < LYNSYN_MAX_SENSORS; i++) {
        file << ";voltage " << i;
      }
      file << "\n";
      
      struct LynsynSample sample;
      while(lynsyn_getNextSample(&sample)) {
        file << lynsyn_cyclesToSeconds(sample.time);
        for(int i = 0; i < MAX_CORES; i++) {
          file << ";" << sample.pc[i];
        }
        for(int i = 0; i < LYNSYN_MAX_SENSORS; i++) {
          file << ";" << sample.current[i];
        }
        for(int i = 0; i < LYNSYN_MAX_SENSORS; i++) {
          file << ";" << sample.voltage[i];
        }
        file << "\n";
      }
    } else {
      printf("Can't open output file\n");
    }

    lynsyn_release();

  } else {
    printf("Can't open lynsyn\n");
  }

  return 0;
}
