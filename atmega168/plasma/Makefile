include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c lib/draw/draw.c lib/draw/matrix/matrix.c lib/twi/twi.c
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

CFLAGS+=-DTWI_BUFFER_LENGTH=384 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DMATRIX_WIDTH=12

include ../../build/targets.mk
