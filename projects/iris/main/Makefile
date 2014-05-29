include ../../../build/avr/variables.mk
include time.mk

PROJECT=ring60
MMCU=atmega328
F_CPU=8000000
SOURCES=main.c lib/remote/remote.c lib/rtc/pcf8563/pcf8563.c lib/twi/twi.c lib/serial/serial.c lib/serial/serial_sync_rx.c lib/serial/serial_sync_tx.c $(time_a_c_sources) $(time_a_asm_sources) 

HFUSE=0xd9
LFUSE=0xe2

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../../build/avr/targets.mk
