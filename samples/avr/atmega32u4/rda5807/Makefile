PROJECT=FM_Tuner
MMCU=atmega32u4
F_CPU=16000000
PROGRAMMER=dfu
CDEFS+=-DTWI_FREQ=100000L
LDFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lc
include ../../../../build/avr.mk
