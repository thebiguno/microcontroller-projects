include ../../build/variables.mk

PROJECT=pumpkin
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/draw/draw.c lib/draw/matrix/matrix.c lib/twi/twi.c bitmaps.c
COMPILER=avr-g++


PROGRAMMER=usbtiny

CFLAGS+=-DTWI_BUFFER_LENGTH=385 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DTWI_MASTER_TX_WRITER -DMATRIX_NO_SHIFT 

include ../../build/targets.mk
