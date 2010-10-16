include ../build/variables.mk

PROJECT=drummaster
MMCU=atmega168
F_CPU=16000000
SOURCES=drummaster.c ../lib/serial/serial_asynchronous.c ../lib/analog/analog_synchronous.c ../lib/timer/timer.c

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DADC_PRESCALER_MASK=5 -DSERIAL_BUFFER_SIZE=512

include ../build/targets.mk
