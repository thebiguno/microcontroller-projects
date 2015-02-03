include ../../build/variables.mk

PROJECT=FM_Tuner
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/i2c/i2c_master.c lib/si4703/si4703.cpp lib/serial/serial.c lib/serial/serial_sync_tx.c
COMPILER=avr-g++

CDEFS+=-DSERIAL_DISABLE_RX

include ../../build/targets.mk
