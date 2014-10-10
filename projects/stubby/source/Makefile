include build/variables.mk

include hardware.mk

ifndef PCB_REVISION
	PCB_REVISION=2
endif

ifndef MAGNETOMETER
	MAGNETOMETER=1
endif

ifndef DISTANCE_SENSOR
	DISTANCE_SENSOR=1
endif

ifndef $(F_CPU)
ifeq '2' '$(PCB_REVISION)'
	F_CPU=20000000
else
	F_CPU=12000000
endif
endif

ifndef TWI_FREQ
	TWI_FREQ=400000L
endif

PROJECT=stubby
MMCU=atmega1284p
COMPILER=avr-g++
SOURCES=Stubby.cpp Leg.cpp \
	controllers/calibration.c controllers/processing.c controllers/universal_controller.c \
	gait/gait_tripod.c \
	hardware/battery.c hardware/magnetometer_HMC5883L.c  hardware/servo.c hardware/status.c hardware/timer2.c \
	types/Point.cpp \
	util/convert.c util/delays.c util/math.c \
	lib/protocol/protocol.c lib/protocol/protocol_serial.c lib/pwm/pwm.c lib/pwm/pwm.S lib/serial/serial.c lib/serial/serial_sync_tx.c lib/twi/twi.c lib/Ring/Ring.cpp
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DTWI_FREQ=$(TWI_FREQ) -DPCB_REVISION=$(PCB_REVISION) -DMAGNETOMETER=$(MAGNETOMETER) -DDISTANCE_SENSOR=$(DISTANCE_SENSOR) -DSERIAL_BUFFER_SIZE=255 -DPWM_MAX_PINS=21 -Wl,-u,vfprintf -lprintf_flt -lm

LFUSE=0xF7
HFUSE=0xD1
EFUSE=0xFE

include build/targets.mk
