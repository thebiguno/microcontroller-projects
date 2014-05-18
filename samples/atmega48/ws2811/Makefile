include ../../build/variables.mk

PROJECT=ws2811
MMCU=atmega48
F_CPU=20000000
SOURCES=main.c lib/ws2811/ws2811.c

HFUSE = 0xDF
LFUSE = 0xFF

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
