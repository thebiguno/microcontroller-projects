include ../../build/variables.mk

PROJECT=pwm
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c lib/pwm/pwm.S
AVRDUDE_PROGRAMMER=arduino

# You can also define anything here and it will override 
# the definitions in variables.mk
CDEFS+=-DPWM_PORTA_UNUSED

include ../../build/targets.mk
