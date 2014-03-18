include build/variables.mk

PROJECT=sump
MMCU=atmega644
F_CPU=20000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

HFUSE=0x91
LFUSE=0xF7

include build/targets.mk
