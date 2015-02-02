include ../../../../build/variables.mk

PROJECT=glcd
MMCU=atmega644
F_CPU=8000000
SOURCES=glcd_st7565r_test.c status_glcd_st7565r.c ../../../lib/glcd/glcd_st7565r.c ../../../lib/glcd/draw.c ../../../lib/glcd/fonts/xsmall.c ../../../lib/glcd/fonts/small.c ../../../lib/glcd/fonts/medium.c ../../../lib/glcd/fonts/large.c ../../../lib/glcd/fonts/xlarge.c ../../../lib/glcd/fonts/ascii.c
PROGRAMMER=usbtiny

HFUSE=0xDF
LFUSE=0xE2

# You can also define anything here and it will override
# the definitions in variables.mk
CFLAGS+=-Wl,-u,vfprintf -lprintf_flt -lm 

include ../../../../build/targets.mk
