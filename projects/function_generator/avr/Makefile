PROJECT=function_generator
MMCU=atmega168
F_CPU=20000000
SOURCES=main.cpp timer1.S lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp lib/Hd44780/CharDisplay.cpp
PROGRAMMER=usbtiny
COMPILER=avr-g++

CDEFS+= -Wl,--gc-sections,-u,vfprintf -lprintf_flt -lc

LFUSE=0xFF
HFUSE=0xDF
EFUSE=0x01

include build/targets.mk
