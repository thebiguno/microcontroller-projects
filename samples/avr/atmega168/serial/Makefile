include ../../build/variables.mk

PROJECT=serial
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c


CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)

include ../../build/targets.mk
