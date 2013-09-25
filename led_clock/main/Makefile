include ../build/variables.mk

PROJECT=clock
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c timer/timer.c clock/clock.c lib/draw/draw.c lib/draw/matrix/matrix.c lib/twi/twi.c
COMPILER=avr-g++
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE
CFLAGS+=-DTWI_BUFFER_LENGTH=385 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DTWI_MASTER_TX_WRITER -DMATRIX_NO_SHIFT 

HFUSE=0xDF
LFUSE=0xDF

include ../build/targets.mk