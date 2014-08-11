include ../../build/variables.mk

PROJECT=pwm
MMCU=atmega168
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c 
#AVRDUDE_PROGRAMMER=usbtiny

CDEFS+=-DPWM_PORTA_UNUSED

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../build/targets.mk
