CFLAGS = -g -O2 -Wall -I/usr/include/libusb-1.0/ -I../mcu/common/ -I../liblynsyn/
LDFLAGS = -lusb-1.0

CC = gcc
CPP = g++
LD = g++

lynsyn_xvc : main.o ../liblynsyn/lynsyn.o
	${LD} $^ ${LDFLAGS} -o $@

%.o : %.c
	${CC} -std=gnu99 ${CFLAGS} -c $< -o $@

%.o : %.cpp
	${CPP} ${CFLAGS} -c $< -o $@

%.o : %.s
	${AS} ${ASFLAGS} -c $< -o $@

%.o : %.S
	${AS} ${ASFLAGS} -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o lynsyn_sampler
