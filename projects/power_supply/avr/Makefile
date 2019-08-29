PROJECT=powersupply
MMCU=atmega32u4
F_CPU=16000000
PROGRAMMER=dfu
CDEFS+=-DADC_PRESCALER_MASK=0x07 -DCHANNEL_COUNT=4 -DTIMER_HARDWARE=0
LDFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lc

include ../../../build/avr.mk
