include ../../build/variables.mk

PROJECT=Oscilloscope
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/timer/timer_1.c lib/serial/serial_async_tx.c lib/serial/serial.c lib/ring/ring.c

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DSERIAL_BUFFER_SIZE=256 -DSERIAL_DISABLE_RX

include ../../build/targets.mk
