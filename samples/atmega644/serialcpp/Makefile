include ../../build/variables.mk

PROJECT=serial
MMCU=atmega644p
F_CPU=20000000
SOURCES=main.c lib/serialcpp/serial.cpp lib/ringcpp/ring.cpp
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DBAUD=$(BAUD)
AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
