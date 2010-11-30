include ../../../../../build/variables.mk

PROJECT=mhv_test
MMCU=atmega168
F_CPU=16000000
SOURCES=../attitude_test.c attitude_mhv.c ../../accel/MMA7455L/accel_MMA7455L.c ../../gyro/LYPR530/gyro_LYPR530.c ../../persist/eeprom/persist_eeprom.c ../../../lib/serial/serial_synchronous.c ../../../lib/i2c/i2c_master.c ../../../lib/eeprom/eeprom.c ../../../lib/analog/analog_asynchronous.c

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -lm -lc -lm

include ../../../../../build/targets.mk
