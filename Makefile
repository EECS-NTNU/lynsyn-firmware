###############################################################################
# Settings for the host tools

export QMAKE = qmake CONFIG+=release

###############################################################################
# Settings for the firmware

export SDK=/opt/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.4/
export TOOLCHAIN=/opt/SimplicityStudio_v4/developer/toolchains/gnu_arm/7.2_2017q4/bin/

# Uncomment one of the following for the HW version you are compiling firmware for:

#export HW=2 # lynsyn original
export HW=3 # lynsyn lite

###############################################################################

.PHONY: host_software
host_software:
	mkdir -p bin
	cd lynsyn_tester && $(MAKE)
	cp lynsyn_tester/lynsyn_tester bin
	cd lynsyn_sampler && $(MAKE)
	cp lynsyn_sampler/lynsyn_sampler bin
	cd lynsyn_xvc && $(MAKE)
	cp lynsyn_xvc/lynsyn_xvc bin
	cd libxsvf && $(MAKE) lynsyn_xsvf
	cp libxsvf/lynsyn_xsvf bin
	mkdir -p lynsyn_viewer/build
	cd lynsyn_viewer/build && $(QMAKE) ..
	cd lynsyn_viewer/build && $(MAKE)
	cp lynsyn_viewer/build/lynsyn_viewer bin

.PHONY: firmware
firmware:
	mkdir -p fwbin
	cd mcu/boot && $(MAKE)
	cp mcu/boot/lynsyn_boot.bin fwbin
	cd mcu/main && $(MAKE)
	cp mcu/main/lynsyn_main.bin fwbin

.PHONY: all
all: host_software firmware

.PHONY: install
install: host_software install_hw
	cp bin/* /usr/bin/
	cp .jtagdevices /etc/jtagdevices

.PHONY: install_hw
install_hw:
	cp udev/48-lynsyn.rules /etc/udev/rules.d
	udevadm control --reload-rules

.PHONY : clean
clean :
	cd mcu/boot && $(MAKE) clean
	cd mcu/main && $(MAKE) clean
	cd lynsyn_tester && $(MAKE) clean
	cd lynsyn_sampler && $(MAKE) clean
	cd lynsyn_xvc && $(MAKE) clean
	cd lynsyn_viewer/build && $(MAKE) clean
	rm -rf liblynsyn/*.o bin fwbin
