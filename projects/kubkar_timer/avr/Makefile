PROJECT=kubkar_timer
MMCU=atmega168
F_CPU=20000000
SOURCES=main.cpp lib/Button/Buttons.cpp lib/analog/analog_synchronous.c
PROGRAMMER=usbtiny
COMPILER=avr-g++
CDEFS+=-DTIMER_BITS=32 

LFUSE=0xFF
HFUSE=0xDF
EFUSE=0x01

include build/targets.mk
