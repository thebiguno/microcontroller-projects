include ../../../build/avr/variables.mk
include time.mk

PROJECT=ring60
MMCU=atmega328
F_CPU=20000000
SOURCES=main.c lib/ws2811/ws2811.c lib/remote/remote.c $(time_a_c_sources) $(time_a_asm_sources) 

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../../build/avr/targets.mk
