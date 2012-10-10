include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega48
F_CPU=24000000
SOURCES=main.c lib/draw/matrix/matrix_display_buffer.c lib/TwoWire/TwoWire.cpp lib/ShiftRegister/ShiftRegister.cpp
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny
#AVRDUDE_PORT=/dev/tty.usbserial*

CFLAGS+=-DTWI_BUFFER_LENGTH=25

include ../../build/targets.mk
