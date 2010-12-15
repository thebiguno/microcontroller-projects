include ../../build/variables.mk

PROJECT=serial
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_async_tx.c

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)

include ../../build/targets.mk
