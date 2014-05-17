include ../../build/variables.mk

PROJECT=ir_recv
MMCU=atmega48
F_CPU=8000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c lib/remote/remote.c

HFUSE = 0xDF
LFUSE = 0xE2

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
