PROJECT=chiindii
MMCU=atmega32u4
F_CPU=16000000
PROGRAMMER=dfu

LDFLAGS=-Wl,-u,vfprintf -lprintf_flt -lm
CDEFS=-DTIMER_BITS=32 -DPWM_PORTA_UNUSED

include ../../../build/avr.mk
