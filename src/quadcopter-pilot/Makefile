# Skeleton Makefile for generic AVR programming.  Use this as a base for 
# future projects.
# 
# How to use:
#  -Change PROJECT to your project name
#  -Change MMCU to your AVR chip name
#  -Change F_CPU to the AVR clock rate in Hz. (e.g. 16000000 for 16MHz)
#  -Add all included libraries to SOURCES
#  -Change AVRDude options as needed (only if you are planning on uploading directly)

PROJECT=Skeleton
MMCU=atmega168
F_CPU=20000000
SOURCES=main.c lib/timer/timer.c

#AVRDude options
AVRDUDE=/opt/local/bin/avrdude
AVRDUDE_CONFIG=/opt/local/etc/avrdude.conf
AVRDUDE_PROGRAMMER=stk500v1
AVRDUDE_PORT=/dev/tty.usb*
AVRDUDE_UPLOAD_RATE=19200

CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-mmcu=$(MMCU) -pedantic -Os -Wall -std=gnu99
CDEFS=-DF_CPU=$(F_CPU)


# Default target.
all: clean build size

build: $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex
	rm -f $(PROJECT).out


$(PROJECT).out: $(SOURCES) 
	$(CC) $(CDEFS) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES)


program: $(PROJECT).hex
	$(AVRDUDE) -V -F -p $(MMCU) -P $(AVRDUDE_PORT) \
		-C $(AVRDUDE_CONFIG) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE)  \
		-U flash:w:$(PROJECT).hex 
	
size: build
	avr-size $(PROJECT).hex

clean:
	rm -f *.o
	rm -f $(PROJECT).hex
