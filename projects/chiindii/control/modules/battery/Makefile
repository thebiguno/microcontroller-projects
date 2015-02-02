include ../../../../build/variables.mk

PROJECT=glcd
MMCU=atmega644p
F_CPU=8000000
SOURCES=battery_test.c battery.c ../../lib/analog/analog_synchronous.c ../../lib/serial1/serial1.c ../../lib/serial1/serial1_sync_tx.c ../../lib/serial1/serial1_sync_rx.c
PROGRAMMER=usbtiny

HFUSE=0xDF
LFUSE=0xE2

# You can also define anything here and it will override
# the definitions in variables.mk
CFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lm 

include ../../../../build/targets.mk
