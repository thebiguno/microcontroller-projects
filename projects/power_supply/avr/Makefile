PROJECT=powersupply
MMCU=atmega32u4
F_CPU=16000000
SOURCES=main.cpp Channel.cpp Encoders.cpp lib/analog/analog_asynchronous.c lib/Button/Buttons.cpp lib/Hd44780/Display.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp
PROGRAMMER=dfu
COMPILER=avr-g++
CDEFS+=-DADC_PRESCALER_MASK=0x07 -DCHANNEL_COUNT=4 -Wl,-u,vfprintf -lprintf_flt -lc


include build/targets.mk
