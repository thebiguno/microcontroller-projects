PROJECT=powersupply
MMCU=atmega32u4
F_CPU=16000000
SOURCES=PowerSupply.cpp Channel.cpp Display.cpp Encoders.cpp State.cpp timer0.c timer1.c usb.c lib/analog/analog_asynchronous.c lib/bootloader/bootloader.c lib/Button/Buttons.cpp lib/Hd44780/CharDisplay.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp lib/usb/rawhid.c lib/twi/twi.c 
PROGRAMMER=dfu
COMPILER=avr-g++
CDEFS+=-DADC_PRESCALER_MASK=0x07 -DCHANNEL_COUNT=4 -DUSB_VENDOR_ID=0x4200 -DUSB_PRODUCT_ID=0xFF01 -DUSB_RAWHID_RX_SIZE=8 -DUSB_RAWHID_TX_SIZE=8 -Wl,-u,vfprintf -lprintf_flt -lc


include build/targets.mk
