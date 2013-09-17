include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c lib/draw/draw.c lib/draw/matrix/matrix.c lib/twi/twi.c lib/analog/analog_synchronous.c lib/timer/timer_1.c
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

CFLAGS+=-DTWI_BUFFER_LENGTH=193 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DTWI_MASTER_TX_WRITER -DMATRIX_NO_SHIFT 

include ../../build/targets.mk
