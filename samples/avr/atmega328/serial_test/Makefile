include ../../build/variables.mk

PROJECT=serial_test
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/serial/serial_sync_rx.c

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbasp

include ../../build/targets.mk
