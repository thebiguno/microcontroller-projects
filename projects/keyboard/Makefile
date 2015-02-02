include ../../build/variables.mk

PROJECT=keyboard
MMCU=atmega1284p
F_CPU=20000000
SOURCES=main.c qwerty.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c

# You can also define anything here and it will override 
# the definitions in variables.mk

#CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)
PROGRAMMER=usbtiny

LFUSE=0xF7
HFUSE=0xD1

include ../../build/targets.mk
