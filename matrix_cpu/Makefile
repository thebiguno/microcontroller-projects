include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c lib/draw/draw.c lib/draw/matrix/matrix_working.c lib/ShiftRegister/ShiftRegister.cpp lib/TwoWire/TwoWire.cpp lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny
#AVRDUDE_PORT=/dev/tty.usbserial*

CFLAGS+=-DTWI_BUFFER_LENGTH=200 -DMATRIX_WORKING_BUFFER_ONLY

include ../../build/targets.mk
