include build/variables.mk

PROJECT=hd44780
MMCU=atmega32u2
F_CPU=16000000
SOURCES=main.c lib/usb/rawhid/usb_rawhid.c lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp
AVRDUDE_PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DUSB_RAWHID_TX_SIZE=32 -DUSB_RAWHID_RX_SIZE=32


include build/targets.mk
