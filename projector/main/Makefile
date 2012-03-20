include ../build/variables.mk

PROJECT=projector
MMCU=atmega168
F_CPU=8000000
SOURCES=main.c shift/shift.c segment/segment.c timer/timer.c display/display.c button/button.c
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE

HFUSE=0xDF
LFUSE=0xDF

include ../build/targets.mk