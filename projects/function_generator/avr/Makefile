include build/variables.mk

PROJECT=function_generator
MMCU=atmega168
F_CPU=20000000
SOURCES=main.cpp lib/Button/Buttons.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct_4bit.cpp pwm.c lib/pwm/pwm.S
AVRDUDE_PROGRAMMER=usbtiny
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+= -Wl,--gc-sections,-u,vfprintf -lprintf_flt -lc -ffunction-sections -fdata-sections -DPWM_MAX_PINS=1 -DPWM_PORTA_UNUSED -DPWM_PORTB_UNUSED -DPWM_PORTD_UNUSED

include build/targets.mk
