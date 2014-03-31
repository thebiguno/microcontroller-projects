include ../../build/variables.mk

PROJECT=hex
MMCU=atmega1284p
F_CPU=10000000
SOURCES=hex.c leg.c servo.c lib/pwm/pwm.c 
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_MAX_PINS=18 -DPWM_PORTD_UNUSED

HFUSE=0xD9
LFUSE=0xFF

include ../../build/targets.mk
