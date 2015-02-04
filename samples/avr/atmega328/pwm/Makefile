PROJECT=pwm
MMCU=atmega328p
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c lib/pwm/pwm.S
PROGRAMMER=arduino

CDEFS+=-DPWM_PORTA_UNUSED

include ../../build/targets.mk
