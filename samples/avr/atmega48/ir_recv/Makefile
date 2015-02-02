include ../../build/variables.mk

PROJECT=ir_recv
MMCU=atmega328
F_CPU=8000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c lib/remote/remote.c

HFUSE = 0xDF
LFUSE = 0xE2


PROGRAMMER=usbtiny

CDEFS=-DF_CPU=$(F_CPU) -DREMOTE_TIMER2

include ../../build/targets.mk
