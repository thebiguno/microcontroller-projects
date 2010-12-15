include ../../build/variables.mk

PROJECT=slave_flash
MMCU=atmega168
#F_CPU=16000000
F_CPU=8000000
SOURCES=main.c lib/analog/analog_asynchronous.c lib/serial/serial.c lib/serial/serial_sync_tx.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_DISABLE_RX

include ../../build/targets.mk
