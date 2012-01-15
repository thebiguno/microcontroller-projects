include ../../build/variables.mk

PROJECT=alarm_clock
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c button.c shift.c lib/i2c/i2c_master.c lib/analog/analog_asynchronous.c

#Comment these next two lines out to make a non-debug version
SOURCES+=lib/serial/serial.c lib/serial/serial_async_tx.c lib/ring/ring.c
CDEFS+=-DDEBUG -DSERIAL_DISABLE_RX

# You can also define anything here and it will override 
# the definitions in variables.mk

#AVRDUDE_PROGRAMMER=usbtiny
CDEFS+=-DADC_PRESCALER_MASK=0x7

include ../../build/targets.mk
