PROJECT=powersupply
MMCU=atmega32u4
F_CPU=16000000
SOURCES=main.cpp lib/analog/analog_asynchronous.c lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct_4bit.cpp
PROGRAMMER=dfu
COMPILER=avr-g++
CDEFS+=-DADC_PRESCALER_MASK=0x07

include build/targets.mk
