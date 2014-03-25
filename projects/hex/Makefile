include ../../build/variables.mk

PROJECT=hex
MMCU=atmega1284p
F_CPU=10000000
SOURCES=hex.c lib/pwm/pwm.c 
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_MAX_PINS=18 -DPWM_ALL_HIGH='PORTA|=0x7F;PORTB|=0x1F;PORTC|=0xFC;'
		

HFUSE=0xD9
LFUSE=0xFF

include ../../build/targets.mk
