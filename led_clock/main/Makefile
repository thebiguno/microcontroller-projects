include ../build/variables.mk

PROJECT=clock
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c timer/timer.c clock/clock.c lib/time/time.c lib/draw/draw.c lib/draw/fonts/cp_ascii.c lib/draw/fonts/f_3X3.c lib/draw/matrix/matrix.c lib/twi/twi.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_async_tx.c lib/Ring/Ring.cpp
COMPILER=avr-g++
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE
CFLAGS+=-DTWI_BUFFER_LENGTH=385 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DTWI_MASTER_TX_WRITER -DMATRIX_NO_SHIFT 

HFUSE=0xDF
LFUSE=0xDF

include ../build/targets.mk