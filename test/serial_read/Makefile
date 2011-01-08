include ../../build/variables.mk

PROJECT=pilot
MMCU=atmega644p
F_CPU=20000000
SOURCES=main.c ../../lib/shift/shift.c ../../lib/serial/serial.c ../../lib/serial/serial_async_rx.c 
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DADC_PRESCALER_MASK=0x7 -DSERIAL_DISABLE_TX -lm

include ../../build/targets.mk
