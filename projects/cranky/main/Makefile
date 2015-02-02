include ../../../build/avr/variables.mk

PROJECT=cranky
MMCU=atmega32u4
F_CPU=16000000
SOURCES=main.c lib/usb/rawhid.c
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

HFUSE=0xDF
LFUSE=0xDF

PROGRAMMER=usbtiny

include ../../../build/avr/targets.mk
