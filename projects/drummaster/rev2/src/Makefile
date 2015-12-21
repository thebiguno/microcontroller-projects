PROJECT=drummaster
TEENSY=31
F_CPU=96000000
GIT_VERSION=$(shell git rev-parse --short HEAD)
CDEFS=-DGIT_VERSION=$(GIT_VERSION)

#Always delete the stats menu, so that the latest git version shows
$(shell rm -f build/menu/Stats.*)

include ../../../../build/teensy.mk
