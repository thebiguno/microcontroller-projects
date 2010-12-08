include ../../build/variables.mk

PROJECT=serial
MMCU=atmega644
F_CPU=20000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)
AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
