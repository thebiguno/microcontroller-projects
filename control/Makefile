include ../build/variables.mk

PROJECT=control
MMCU=atmega644p
F_CPU=6000000
SOURCES=main.c ../lib/timer/timer_0.c ../lib/ring/ring.c
AVRDUDE_PROGRAMMER=usbtiny

HFUSE=0x91
LFUSE=0xF7

# You can also define anything here and it will override 
# the definitions in variables.mk

CLIBS+=-lm

#Include each module
include modules/comm/module.mk
include modules/control/module.mk
include modules/protocol/module.mk
include modules/util/module.mk

include ../build/targets.mk
