include build/variables.mk

PROJECT=function_generator
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.cpp square.S lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct_4bit.cpp
AVRDUDE_PROGRAMMER=arduino
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=


include build/targets.mk
