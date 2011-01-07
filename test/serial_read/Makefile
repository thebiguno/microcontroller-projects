include ../../build/variables.mk

PROJECT=pilot
MMCU=atmega644p
F_CPU=20000000
SOURCES=main.c ../../lib/timer/timer_0.c ../../lib/shift/shift.c ../../lib/serial/serial.c ../../lib/serial/serial_async_rx.c ../../lib/serial/serial_sync_tx.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DADC_PRESCALER_MASK=0x7 -DNO_INTERRUPT_ENABLE -lm

include ../../build/targets.mk
