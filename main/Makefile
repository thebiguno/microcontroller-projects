include ../build/variables.mk

PROJECT=led
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c shift/shift.c segment/segment.c matrix/matrix.c timer/timer.c clock/clock.c button/button.c
CDEFS+=-DNO_INTERRUPT_ENABLE

include ../build/targets.mk

