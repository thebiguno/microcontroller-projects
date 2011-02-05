include ../build/variables.mk

PROJECT=pilot
MMCU=atmega644p
F_CPU=20000000
SOURCES=main.c ../lib/timer/timer_0.c ../lib/shift/shift.c ../lib/ring/ring.c
AVRDUDE_PROGRAMMER=usbtiny

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DADC_PRESCALER_MASK=0x7 -DNO_INTERRUPT_ENABLE -lm

#Include each module
include modules/status/module.mk
include modules/accel/module.mk
include modules/attitude/module.mk
include modules/comm/module.mk
include modules/esc/module.mk
include modules/gyro/module.mk
include modules/motor/module.mk
include modules/persist/module.mk
include modules/pid/module.mk
include modules/protocol/module.mk
include modules/util/module.mk

include ../build/targets.mk
