include ../../../build/variables.mk

PROJECT=serial_test
MMCU=atmega644
F_CPU=20000000
SOURCES=main.c protocol.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/serial/serial_async_rx.c

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../../build/targets.mk
