include ../../build/variables.mk

PROJECT=distance
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/Distance/Distance.cpp
PROGRAMMER=arduino
COMPILER=avr-g++
CDEFS+=-DSERIAL_DISABLE_RX


include ../../build/targets.mk
