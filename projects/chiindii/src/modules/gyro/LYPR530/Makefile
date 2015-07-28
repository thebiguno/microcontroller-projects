include ../../../../../build/variables.mk

PROJECT=LYPR530_test
MMCU=atmega644
F_CPU=20000000
SOURCES=LYPR530_test.c gyro_LYPR530.c ../../persist/eeprom/persist_eeprom.c ../../../lib/eeprom/eeprom.c ../../status/status_shift.c ../../../lib/shift/shift.c ../../../lib/serial/serial.c ../../../lib/serial/serial_async_rx.c ../../../lib/serial/serial_async_tx.c ../../../lib/analog/analog_asynchronous.c
PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -Wl,-u,vfprintf -lprintf_flt -lm

include ../../../../../build/targets.mk
