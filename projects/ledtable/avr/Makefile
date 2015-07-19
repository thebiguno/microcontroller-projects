PROJECT=led_table
MMCU=atmega32u4
F_CPU=16000000
SOURCES=LedTable.cpp Matrix.cpp Life.cpp Tictactoe.cpp Tetris.cpp Mood.cpp Clock.cpp Animated.cpp lib/Draw/Draw.cpp lib/Draw/Rgb.cpp lib/Draw/Hsv.cpp lib/draw/fonts/f_3x5.c lib/draw/fonts/cp_ascii_caps.c lib/ws281x/ws281x_w16.c lib/twi/twi.c lib/rtc/mcp79410/mcp79410.c lib/Psx/AbstractPsx.cpp lib/Psx/Psx.cpp
PROGRAMMER=dfu
COMPILER=avr-g++

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS+=-DUSB_STRING_DESCRIPTOR_LENGTH=21 -DUSB_STR_MANUFACTURER=L\"Warren\ Janssens\" -DUSB_STR_PRODUCT=L\"Led\ Table\" -DUSB_VENDOR_ID=0x574a -DUSB_PRODUCT_ID=0x6c74
CDEFS+=-DWS281X_PORT=PORTB -DWS281X_PIN=2
include build/targets.mk
