include ../../../../../build/variables.mk

PROJECT=LYPR530_test
MMCU=atmega644
F_CPU=20000000
SOURCES=LYPR530_test.c gyro_LYPR530.c ../../../lib/serial/serial_synchronous.c ../../../lib/analog/analog_asynchronous.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -Wl,-u,vfprintf -lprintf_flt -lm

include ../../../../../build/targets.mk
