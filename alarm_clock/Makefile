include ../../build/variables.mk

PROJECT=alarm_clock
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c button.c shift.c lib/i2c/i2c_master.c 

#Comment these next two lines out to make a non-debug version
SOURCES+=lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c 
CDEFS+=-DDEBUG

# You can also define anything here and it will override 
# the definitions in variables.mk

#AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
