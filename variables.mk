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

#AVRDude options
AVRDUDE=avrdude
AVRDUDE_PROGRAMMER=stk500v1
AVRDUDE_PORT=/dev/tty.usb*
AVRDUDE_UPLOAD_RATE=19200

HFUSE=0x99
LFUSE=0x42

COMPILER=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall -std=gnu99 -mcall-prologues
CDEFS=-DF_CPU=$(F_CPU)
