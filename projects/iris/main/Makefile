include ../../../build/avr/variables.mk
include time.mk

PROJECT=iris
MMCU=atmega328
F_CPU=8000000
SOURCES=main.c lib/remote/remote.c lib/rtc/ds1307/ds1307.c lib/twi/twi.c $(time_a_c_sources) $(time_a_asm_sources) 

HFUSE=0xd9
LFUSE=0xe2

CDEFS += -DREMOTE_TIMER2 -DREMOTE_INT1

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../../build/avr/targets.mk
