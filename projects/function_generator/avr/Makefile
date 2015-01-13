include build/variables.mk

PROJECT=function_generator
MMCU=atmega168
F_CPU=20000000
SOURCES=main.cpp lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct_4bit.cpp
AVRDUDE_PROGRAMMER=usbtiny
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=


include build/targets.mk
