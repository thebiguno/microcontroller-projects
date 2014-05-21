include ../../build/variables.mk

PROJECT=i2c
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/i2c/i2c_master.c lib/serial/serial.c lib/serial/serial_sync_tx.c lib/ring/ring.c

# You can also define anything here and it will override
# the definitions in variables.mk

CDEFS+=-DSERIAL_DISABLE_RX

include ../../build/targets.mk

