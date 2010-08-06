include ../../build/variables.mk

PROJECT=control
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c

# You can also define anything here and it will override 
# the definitions in variables.mk

#Include each module
include modules/comm/module.mk
include modules/control/module.mk

include ../../build/targets.mk