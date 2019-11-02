PROJECT=lego_pf
MMCU=atmega328
F_CPU=16000000
PROGRAMMER=arduino
AVRDUDE_PORT=/dev/ttyUSB0
AVRDUDE_BAUD=57600
CDEFS+=-DDEBUG
LDFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lc

include ../../../build/avr.mk
