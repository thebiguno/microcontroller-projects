include build/variables.mk

PROJECT=stubby
MMCU=atmega1284p
COMPILER=avr-g++
F_CPU=12000000
SOURCES=Stubby.cpp Leg.cpp Point.cpp \
	controllers/processing.c controllers/universal_controller.c \
	gait/gait_tripod.c \
	hardware/servo.c hardware/status.c \
	util/convert.c util/math.c \
	lib/protocol/protocol.c lib/protocol/protocol_serial.c lib/pwm/pwm.c lib/pwm/pwm.S lib/serial/serial.c lib/serial/serial_sync_tx.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_MAX_PINS=21 -lm

LFUSE=0xF7
HFUSE=0xD1
EFUSE=0xFE

include build/targets.mk
