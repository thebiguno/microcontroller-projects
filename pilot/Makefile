include ../../build/variables.mk

PROJECT=pilot
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DNO_INTERRUPT_ENABLE

#Include each module
include modules/accel/module.mk
include modules/attitude/module.mk
include modules/comm/module.mk
include modules/esc/module.mk
include modules/gyro/module.mk
include modules/motor/module.mk
include modules/persist/module.mk
include modules/pid/module.mk

include ../../build/targets.mk
