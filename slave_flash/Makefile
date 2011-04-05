include build/variables.mk

PROJECT=slave_flash
MMCU=attiny85
F_CPU=8000000
SOURCES=main.c lib/analog/analog_synchronous.c lib/timer/timer_0.c 
#lib/serial/serial_sync_tx.c lib/serial/serial.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_DISABLE_RX -DADC_PRESCALER_MASK=0x6

include build/targets.mk
