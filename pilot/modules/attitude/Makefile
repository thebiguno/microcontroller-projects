include ../../../../build/variables.mk

PROJECT=MMA7455L_test
MMCU=atmega168
F_CPU=16000000
SOURCES=attitude_test.c comp/attitude_complementary.c ../accel/MMA7455L/accel_MMA7455L.c ../gyro/LYPR530/gyro_LYPR530.c ../persist/eeprom/persist_eeprom.c ../../lib/serial/serial_synchronous.c ../../lib/i2c/i2c_master.c ../../lib/timer/timer_0.c ../../lib/eeprom/eeprom.c ../../lib/analog/analog_asynchronous.c 

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -Wl,-u,vfprintf -lprintf_flt -lm 

include ../../../../build/targets.mk