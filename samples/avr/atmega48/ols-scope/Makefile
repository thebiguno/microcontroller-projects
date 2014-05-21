include ../../build/variables.mk

PROJECT=ols-scope
MMCU=atmega48
F_CPU=20000000
SOURCES=main.c lib/analog/analog_synchronous.c
AVRDUDE_PROGRAMMER=usbtiny

HFUSE=0xDF
LFUSE=0x7F
EFUSE=0xFF

# CDEFS+=-DADC_SLEEP_MODE

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../build/targets.mk
