PROJECT=matrix
MMCU=atmega48
F_CPU=24000000
SOURCES=main.c lib/twi/twi.c ShiftRegister.cpp
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

PROGRAMMER=usbtiny

CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall -DTWI_BUFFER_LENGTH=385 -DTWI_DISABLE_SLAVE_TX -DTWI_DISABLE_MASTER -DTWI_CUSTOM_BUFFERS -DTWI_SLAVE_RX_READER

#Fuses for ATMega48
LFUSE=0xF7
HFUSE=0xDC
EFUSE=0xFF


include build/targets.mk
