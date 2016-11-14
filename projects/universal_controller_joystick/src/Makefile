PROJECT=drummaster
TEENSY=LC
F_CPU=48000000
GIT_VERSION=$(shell git rev-parse --short HEAD)
CDEFS=-DGIT_VERSION=$(GIT_VERSION)
OPTIONS=-DUSB_SERIAL_HID -DLAYOUT_US_ENGLISH

#Always delete the stats menu, so that the latest git version shows
$(shell rm -f build/menu/Stats.*)

include ../../../build/teensy.mk
