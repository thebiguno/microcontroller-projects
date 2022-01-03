PROJECT=alarmclock
MMCU=atmega32u4
F_CPU=16000000
PROGRAMMER=dfu
CDEFS+=-DTIMER_HARDWARE=0
#Add -DDEBUG to the CDEFS to enable debug mode
LDFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lc

include ../../../build/avr.mk
