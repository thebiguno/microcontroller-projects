include build/variables.mk

PROJECT=hd44780
MMCU=atmega32u2
F_CPU=16000000
SOURCES=main.c usb_rawhid.c lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp
AVRDUDE_PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

include build/targets.mk
