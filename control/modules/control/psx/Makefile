include ../../../../../build/variables.mk

PROJECT=psx_test
MMCU=atmega644p
F_CPU=20000000
SOURCES=psx_test.c control_psx.c ../../../lib/psx/psx.c ../../../lib/serial1/serial1.c ../../../lib/serial1/serial1_sync_rx.c ../../../lib/serial1/serial1_sync_tx.c
AVRDUDE_PROGRAMMER=usbtiny

# The flags after -Wl... are to enable double format strings in printf
CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall -std=gnu99 -Wl,-u,vfprintf -lprintf_flt -lm 

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../../../../build/targets.mk