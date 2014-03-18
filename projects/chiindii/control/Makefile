include ../build/variables.mk

PROJECT=control
MMCU=atmega644p
F_CPU=8000000
SOURCES=main.c ../lib/timer/timer_0.c ../lib/Ring/Ring.cpp
AVRDUDE_PROGRAMMER=usbtiny
COMPILER=avr-g++

HFUSE=0x91
LFUSE=0xF7

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS += -DADC_PRESCALER_MASK=0x7 -DNO_INTERRUPT_ENABLE
CLIBS+=-lm -Wl,-u,vfprintf -lprintf_flt -lm 
CFLAGS += -Wl,--relax,--gc-sections
GIT_VERSION=$(shell git describe --long --tags)

#Include each module
include modules/battery/module.mk
include modules/comm/module.mk
include modules/control/module.mk
include modules/protocol/module.mk
include modules/status/module.mk
include modules/util/module.mk

all: filterversion

filterversion:
	@mv main.h main.h.orig
	@sed -e s/--GIT_VERSION--/$(GIT_VERSION)/g main.h.orig > main.h

include ../build/targets.mk

all: cleanupfilterversion

cleanupfilterversion:
	@mv main.h.orig main.h
