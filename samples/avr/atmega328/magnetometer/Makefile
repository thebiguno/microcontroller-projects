include ../../build/variables.mk

PROJECT=magnetometer
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/twi/twi.c lib/magnetometer/magnetometer.c
PROGRAMMER=arduino
CDEFS+=-DSERIAL_DISABLE_RX -Wl,-u,vfprintf -lprintf_flt -lm


include ../../build/targets.mk
