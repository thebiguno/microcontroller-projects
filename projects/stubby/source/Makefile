include build/variables.mk

PROJECT=stubby
MMCU=atmega1284p
COMPILER=avr-g++
F_CPU=12000000
SOURCES=Stubby.cpp Leg.cpp calibration.c comm.c status.c lib/pwm/pwm.c lib/serial/serial.c lib/serial/serial_sync_tx.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_MAX_PINS=21

HFUSE=0xD1
LFUSE=0xFF
EFUSE=0xFD

include build/targets.mk
