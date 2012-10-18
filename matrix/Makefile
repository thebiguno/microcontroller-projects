include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega48
F_CPU=24000000
SOURCES=main.c lib/draw/matrix/matrix_display.c lib/twi/twi.c lib/ShiftRegister/ShiftRegister.cpp
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny
#AVRDUDE_PORT=/dev/tty.usbserial*

CFLAGS+=-DTWI_BUFFER_LENGTH=25 -DTWI_DISABLE_SLAVE_TX -DTWI_DISABLE_MASTER

#Fuses for ATMega48
LFUSE=0xF7
HFUSE=0xDC
EFUSE=0xFF


include ../../build/targets.mk
