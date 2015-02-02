include ../../build/variables.mk

PROJECT=serial
MMCU=atmega644p
F_CPU=20000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_async_tx.c


CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)
PROGRAMMER=usbtiny

include ../../build/targets.mk
