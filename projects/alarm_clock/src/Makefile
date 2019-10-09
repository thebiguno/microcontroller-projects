PROJECT=alarmclock
MMCU=atmega32u4
F_CPU=16000000
PROGRAMMER=dfu
CDEFS+=-DTIMER_HARDWARE=0 -DADC_HARDWARE=2 -DADC_PRESCALER_MASK=0x07
LDFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lc

include ../../../build/avr.mk
