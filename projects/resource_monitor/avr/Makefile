PROJECT=resource_monitor
MMCU=atmega32u2
F_CPU=16000000
SOURCES=main.c lib/pwm/pwm.c lib/pwm/pwm.S lib/usb/rawhid.c locallib/Hd44780/Hd44780.cpp locallib/Hd44780/Hd44780_Direct.cpp
PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DPWM_PORTA_UNUSED -DUSB_VENDOR_ID=0x4200 -DUSB_PRODUCT_ID=0xFF00


include build/targets.mk
