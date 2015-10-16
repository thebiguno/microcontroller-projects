PROJECT=led_cubicle
MMCU=atmega168
F_CPU=16000000
PROGRAMMER=usbtiny

CFLAGS+=-DTWI_BUFFER_LENGTH=385 -DTWI_NO_SLAVE -DTWI_CUSTOM_BUFFERS

# You can also define anything here and it will override 
# the definitions in variables.mk

include ../../../build/avr.mk
