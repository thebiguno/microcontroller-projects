include build/variables.mk

PROJECT=function_generator
MMCU=atmega168
F_CPU=20000000
SOURCES=main.cpp timer1.S lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct_4bit.cpp
AVRDUDE_PROGRAMMER=usbtiny
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+= -Wl,--gc-sections,-u,vfprintf -lprintf_flt -lc

LFUSE=0xFF
HFUSE=0xDF
EFUSE=0x01

include build/targets.mk
