include ../build/variables.mk

PROJECT=projector
MMCU=atmega48
F_CPU=1000000
SOURCES=main.c shift/shift.c segment/segment.c timer/timer.c button/button.c
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE

HFUSE=0xDF
LFUSE=0x62

include ../build/targets.mk