include ../../build/variables.mk

PROJECT=matrix
MMCU=atmega168
F_CPU=24000000
SOURCES=main.c lib/draw/draw.c lib/draw/matrix/matrix.c lib/Shift/Shift.cpp lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c lib/draw/fonts/tall.c lib/timer/timer_1.c
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny
#AVRDUDE_PORT=/dev/tty.usbserial*

#CFLAGS+=-D=NO_INTERRUPT_ENABLE

include ../../build/targets.mk
