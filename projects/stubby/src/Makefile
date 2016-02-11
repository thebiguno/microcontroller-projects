# Most user configurable options are in this file.
include hardware.mk

ifndef AVRDUDE_PROGRAMMER
	AVRDUDE_PROGRAMMER=usbtiny
endif

ifneq 'usbtiny' '$(AVRDUDE_PROGRAMMER)'
ifndef AVRDUDE_PORT
ifeq ($(OS),Linux)
	AVRDUDE_PORT=/dev/ttyUSB0
else
$(error Please define AVRDUDE_PORT in hardware.mk)
endif
endif
endif

ifndef PCB_REVISION
	PCB_REVISION=2
endif

ifndef MAGNETOMETER
	MAGNETOMETER=1
	ifndef MAGNETOMETER_ORIENTATION_OFFSET
		MAGNETOMETER_ORIENTATION_OFFSET=3.1415926
	endif
endif

ifndef DISTANCE_SENSOR
	DISTANCE_SENSOR=1
endif

ifndef F_CPU
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

LDFLAGS=-Wl,-u,vfprintf -lprintf_flt -lm
CDEFS=-DTWI_FREQ=$(TWI_FREQ) -DPCB_REVISION=$(PCB_REVISION) -DMAGNETOMETER=$(MAGNETOMETER) -DMAGNETOMETER_ORIENTATION_OFFSET=$(MAGNETOMETER_ORIENTATION_OFFSET) -DDISTANCE_SENSOR=$(DISTANCE_SENSOR) -DPWM_MAX_PINS=21 -DTIMER_BITS=32

include ../../../build/avr.mk
