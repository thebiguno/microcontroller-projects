include build/variables.mk

PROJECT=resource_monitor
MMCU=atmega32u2
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c lib/pwm/pwm.S lib/usb/rawhid/usb_rawhid.c lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp
AVRDUDE_PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_PORTA_UNUSED -DUSB_VENDOR_ID=0x4242 -DUSB_PRODUCT_ID=0xFF00


include build/targets.mk
