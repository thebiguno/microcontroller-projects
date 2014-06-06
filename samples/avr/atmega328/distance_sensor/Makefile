include ../../build/variables.mk

PROJECT=distance
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/Distance/Distance.cpp
AVRDUDE_PROGRAMMER=arduino
COMPILER=avr-g++
CDEFS+=-DSERIAL_DISABLE_RX

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../build/targets.mk
