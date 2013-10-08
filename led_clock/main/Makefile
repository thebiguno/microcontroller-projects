include ../build/variables.mk

PROJECT=clock
MMCU=atmega168
SOURCES=main.c timer/timer.c clock/clock.c lib/time/time.c lib/draw/draw.c lib/draw/fonts/cp_ascii.c lib/draw/fonts/f_3X5.c lib/draw/matrix/matrix.c lib/twi/twi.c lib/serial/serial.c lib/serial/serial_async_rx.c lib/serial/serial_async_tx.c lib/Ring/Ring.cpp
COMPILER=avr-g++
AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DNO_INTERRUPT_ENABLE
CFLAGS+=-DTWI_BUFFER_LENGTH=385 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS -DTWI_MASTER_TX_WRITER -DMATRIX_NO_SHIFT 

HFUSE=0xDF
LFUSE=0xDF

ifndef F_CPU 
$(error F_CPU is undefined.  Use syntax 'make F_CPU=xx000000' to supply clock speed.)
endif

include ../build/targets.mk
