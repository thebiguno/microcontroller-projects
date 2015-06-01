PROJECT=led_table
MMCU=atmega32u4
F_CPU=16000000
SOURCES=LedTable.cpp Life.cpp matrix.c lib/ws2811/ws281x_w16.c lib/psx/psx.c lib/timer/timer_0.c lib/usb/rawhid.c
PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DUSB_STRING_DESCRIPTOR_LENGTH=21 -DUSB_STR_MANUFACTURER=L\"Warren\ Janssens\" -DUSB_STR_PRODUCT=L\"Led\ Table\" -DUSB_VENDOR_ID=0x574a -DUSB_PRODUCT_ID=0x6c74
CDEFS+=-DWS2811_PORT=PORTB
include build/targets.mk
