PROJECT=pwm
MMCU=atmega32u4
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c 
PROGRAMMER=dfu

CDEFS+=-DPWM_PORTA_UNUSED 

include ../../build/targets.mk
