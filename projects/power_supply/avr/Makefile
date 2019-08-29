PROJECT=powersupply
MMCU=atmega32u4
F_CPU=16000000
SOURCES=PowerSupply.cpp Channel.cpp Display.cpp Encoders.cpp State.cpp timer0.c timer1.c lib/analog/analog_asynchronous.c lib/bootloader/bootloader.c lib/Button/Buttons.cpp lib/Hd44780/CharDisplay.cpp lib/Hd44780/Hd44780.cpp lib/Hd44780/Hd44780_Direct.cpp lib/twi/twi.c
PROGRAMMER=dfu
COMPILER=avr-g++
CDEFS+=-DADC_PRESCALER_MASK=0x07 -DCHANNEL_COUNT=4 -Wl,-u,vfprintf -lprintf_flt -lc


include build/targets.mk
