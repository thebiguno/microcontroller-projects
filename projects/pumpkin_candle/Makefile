include ../../build/variables.mk

PROJECT=candle
MMCU=attiny85
F_CPU=1000000
#F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c
CLIBS += -lm

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../build/targets.mk
