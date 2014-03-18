include ../../build/variables.mk

PROJECT=alarm_clock_2
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_tx
#lib/mp3/mp3.cpp
COMPILER=avr-g++

#Comment these next two lines out to make a non-debug version
#SOURCES+=lib/serial/serial.c lib/serial/serial_async_tx.c lib/ring/ring.c
CDEFS+=-DSERIAL_DISABLE_RX

# You can also define anything here and it will override 
# the definitions in variables.mk

#AVRDUDE_PROGRAMMER=usbtiny
#CDEFS+=-DADC_PRESCALER_MASK=0x7

LFUSE=0xE2
HFUSE=0xDF
EFUSE=0xF9

include ../../build/targets.mk
