include ../../build/variables.mk

PROJECT=keyboard
MMCU=atmega1284p
F_CPU=20000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_async_tx.c lib/ring/ring.c

# You can also define anything here and it will override 
# the definitions in variables.mk

#CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)
AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
