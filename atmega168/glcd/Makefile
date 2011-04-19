include ../../build/variables.mk

PROJECT=glcd
MMCU=atmega168
F_CPU=8000000
SOURCES=main.c lib/glcd/glcd_st7565r.c lib/glcd/draw.c lib/glcd/fonts/tiny.c
AVRDUDE_PROGRAMMER=usbtiny

HFUSE=0xDF
LFUSE=0xE2

# You can also define anything here and it will override
# the definitions in variables.mk

include ../../build/targets.mk

