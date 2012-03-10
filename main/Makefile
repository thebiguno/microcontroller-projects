include ../build/variables.mk

PROJECT=led
MMCU=atmega168
F_CPU=9216000
SOURCES=main.c shift/shift.c segment/segment.c timer/timer.c clock/clock.c display/display.c button/button.c life/life.c
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE

HFUSE=0xDF
LFUSE=0xDF

include ../build/targets.mk