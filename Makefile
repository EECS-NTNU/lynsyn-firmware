###############################################################################
#
# Lynsyn Makefile
#
# Compiles firmware for all Lynsyn boards
#
# Usage:
#    make            Builds firmware
#    make synthesis  Builds FPGA bitfile (for Lynsyn Original)
#    make all        Builds everything
#    make clean      Cleans everything
#
# Make sure to adjust settings at the top of this file for your system
#
###############################################################################

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
###############################################################################
# Everything below this should not need any changes

ifeq ($(HW),2)
alldeps=bin/lynsyn_boot.bin bin/lynsyn_main.bin synthesis
else
alldeps=bin/lynsyn_boot.bin bin/lynsyn_main.bin
endif

###############################################################################

.PHONY: all
all: $(alldeps)
	@echo
	@echo "Compilation successful"
	@echo

###############################################################################

.PHONY: bin/lynsyn_boot.bin
bin/lynsyn_boot.bin:
	mkdir -p bin
	cd mcu/boot && $(MAKE)
	cp mcu/boot/lynsyn_boot.bin bin

.PHONY: bin/lynsyn_main.bin
bin/lynsyn_main.bin:
	mkdir -p bin
	cd mcu/main && $(MAKE)
	cp mcu/main/lynsyn_main.bin bin

###############################################################################

.PHONY: synthesis
synthesis:
	mkdir -p bin
	cd fpga && $(MAKE)
	cp fpga/build/lynsyn.mcs bin
	@echo
	@echo "FPGA synthesis successful"
	@echo

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
clean:
	cd mcu/boot && $(MAKE) clean
	cd mcu/main && $(MAKE) clean
	rm -rf bin
