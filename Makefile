###############################################################################
# Settings for the host tools

export QMAKE = qmake CONFIG+=release

###############################################################################
# Settings for the firmware

export SDK=/opt/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/
export TOOLCHAIN=/opt/SimplicityStudio_v4/developer/toolchains/gnu_arm/7.2_2017q4/bin/

# Uncomment one of the following for the HW version you are compiling firmware for:

# compile for lynsyn original
#export HW=2

# compile for lynsyn lite
export HW=3

###############################################################################

ifeq ($(HW),2)
alldeps=host_software firmware synthesis
else
alldeps=host_software firmware
endif

ifeq ($(OS),Windows_NT)
export CFLAGS = -I../../argp-standalone-1.3/ -I/mingw64/include/libusb-1.0/
export LDFLAGS = -largp -L../../argp-standalone-1.3/
HOST_EXECUTABLES=bin/lynsyn_tester bin/lynsyn_sampler bin/lynsyn_xsvf bin/lynsyn_viewer
else
HOST_EXECUTABLES=bin/lynsyn_tester bin/lynsyn_sampler bin/lynsyn_xvc bin/lynsyn_xsvf bin/lynsyn_viewer
endif

export CFLAGS += -g -O2 -Wall -I/usr/include/libusb-1.0/ -I../mcu/common/ -I../liblynsyn/ 
export LDFLAGS += -lusb-1.0 
export CXXFLAGS = -std=c++11 

export CC = gcc
export CPP = g++
export LD = g++
export AR = ar
export RANLIB = ranlib

###############################################################################

.PHONY: host_software
host_software: $(HOST_EXECUTABLES)
	@echo
	@echo "Host software compilation successful"
	@echo

bin/lynsyn_tester:
	mkdir -p bin
	cd lynsyn_tester && $(MAKE)
	cp lynsyn_tester/lynsyn_tester bin

bin/lynsyn_sampler:
	cd lynsyn_sampler && $(MAKE)
	cp lynsyn_sampler/lynsyn_sampler bin

bin/lynsyn_xvc:
	cd lynsyn_xvc && $(MAKE)
	cp lynsyn_xvc/lynsyn_xvc bin

bin/lynsyn_xsvf:
	cd libxsvf && $(MAKE) lynsyn_xsvf
	cp libxsvf/lynsyn_xsvf bin

bin/lynsyn_viewer:
	mkdir -p lynsyn_viewer/build
	cd lynsyn_viewer/build && $(QMAKE) ..
	cd lynsyn_viewer/build && $(MAKE)
ifeq ($(OS),Windows_NT)
	cp lynsyn_viewer/build/release/lynsyn_viewer bin
else
	cp lynsyn_viewer/build/lynsyn_viewer bin
endif

###############################################################################

.PHONY: firmware
firmware: fwbin/lynsyn_boot.bin fwbin/lynsyn_main.bin
	@echo
	@echo "Firmware compilation successful"
	@echo

fwbin/lynsyn_boot.bin:
	mkdir -p fwbin
	cd mcu/boot && $(MAKE)
	cp mcu/boot/lynsyn_boot.bin fwbin

fwbin/lynsyn_main.bin:
	mkdir -p fwbin
	cd mcu/main && $(MAKE)
	cp mcu/main/lynsyn_main.bin fwbin

###############################################################################

.PHONY: synthesis
synthesis:
	mkdir -p fwbin
	cd fpga && $(MAKE)
	cp fpga/build/lynsyn.mcs fwbin
	@echo
	@echo "FPGA synthesis successful"
	@echo

###############################################################################

.PHONY: all
all: $(alldeps)

###############################################################################

.PHONY: install
install: host_software install_hw
	cp bin/* /usr/bin/
	cp .jtagdevices /etc/jtagdevices
	@echo
	@echo "Software and hardware installed"
	@echo

###############################################################################

.PHONY: install_hw
install_hw:
	cp udev/48-lynsyn.rules /etc/udev/rules.d
	udevadm control --reload-rules

###############################################################################

.PHONY : clean
clean : fwclean hostclean

.PHONY : fwclean
fwclean :
	cd mcu/boot && $(MAKE) clean
	cd mcu/main && $(MAKE) clean

.PHONY : hostclean
hostclean:
	cd lynsyn_tester && $(MAKE) clean
	cd lynsyn_sampler && $(MAKE) clean
	cd libxsvf && $(MAKE) clean
	cd lynsyn_xvc && $(MAKE) clean
	rm -rf lynsyn_viewer/build
	cd fpga && $(MAKE) clean
	rm -rf liblynsyn/*.o bin fwbin
