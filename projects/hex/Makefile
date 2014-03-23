include ../../build/variables.mk

PROJECT=slave_flash
MMCU=atmega1284p
F_CPU=10000000
SOURCES=main.c lib/pwm/pwm.c 
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_MAX_PINS=18

HFUSE=0xD9
LFUSE=0xFF

include ../../build/targets.mk
