include ../../../../../build/variables.mk

PROJECT=xbee_test
MMCU=atmega168
F_CPU=16000000
SOURCES=xbee_test.c communication_xbee.c ../../../lib/serial/serial.c

# The flags after -Wl... are to enable double format strings in printf
CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall -std=gnu99 -Wl,-u,vfprintf -lprintf_flt -lm 

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../../../../build/targets.mk