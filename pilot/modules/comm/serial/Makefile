include ../../../../../build/variables.mk

PROJECT=serial_test
MMCU=atmega168
F_CPU=16000000
SOURCES=serial_test.c communication_serial.c ../../../lib/serial/serial_synchronous.c

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -Wl,-u,vfprintf -lprintf_flt -lm 

include ../../../../../build/targets.mk
