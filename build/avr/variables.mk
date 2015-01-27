# Common Makefile for generic AVR programming.  Include this file
# from stub Makefiles on a per-project basis.
# 
# In the sub makefile you need to include:
#  PROJECT=<Project_Name>
#  MMCU=<AVR_Tyle>
#  F_CPU=<Clock_Rate> (in Hz, e.g. 16000000 for 16MHz)
#  SOURCES=<Sources> (All .c sources to compile and link together)
#  include <Path_To_Makefile_common>
######################
OS=$(shell uname -s)

#AVRDude options
ifeq ($(OS),Linux)
	AVRDUDE_PORT=/dev/ttyUSB0
else
	AVRDUDE_PORT=/dev/tty.usbserial*
endif
DFU=dfu-programmer
AVRDUDE=avrdude
AVRDUDE_PROGRAMMER=stk500v1

OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
READELF=avr-readelf
AVRSIZE=avr-size
CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall
CDEFS=-DF_CPU=$(F_CPU)
#CLIBS=
