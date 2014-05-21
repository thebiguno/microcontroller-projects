include ../../build/variables.mk

PROJECT=i2c
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/twi/twi.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/serial/serial_sync_rx.c
COMPILER=avr-g++

# You can also define anything here and it will override
# the definitions in variables.mk
CDEFS+=-DTWI_BUFFER_LENGTH=128

include ../../build/targets.mk

