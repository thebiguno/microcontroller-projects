include build/variables.mk

PROJECT=PSXController
MMCU=atmega48
F_CPU=8000000
SOURCES=main.c lib/psx/psx.c lib/serial/serial.c lib/protocol/protocol.c lib/serial/serial_sync_tx.c
PROGRAMMER=usbtiny

HFUSE=0xDF
LFUSE=0xE2

# You can also define anything here and it will override
# the definitions in variables.mk

include build/targets.mk

dist:
	tar -czvf psx_controller_source.tgz Makefile build/* main.c psx_controller_client.py lib/serial/serial.h lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c lib/psx/psx.h lib/psx/psx.c
