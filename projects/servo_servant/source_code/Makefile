include ../../../build/avr/variables.mk

PROJECT=servo_servant
MMCU=atmega32u2
F_CPU=16000000
SOURCES=main.c
AVRDUDE_PROGRAMMER=dfu

# You can also define anything here and it will override 
# the definitions in variables.mk
LFUSE=0xDE
HFUSE=0xD9
EFUSE=0xF4

include ../../../build/avr/targets.mk
