include ../../../../build/avr/variables.mk

PROJECT=drummaster
MMCU=atmega328p
F_CPU=16000000
COMPILER=avr-g++
SOURCES=drummaster.c lib/protocol/protocol.c lib/protocol/protocol_serial.c lib/serial/serial.c lib/serial/serial_sync_tx.c
PROGRAMMER=arduino

# You can also define anything here and it will override 
# the definitions in variables.mk

CDEFS += -DADC_PRESCALER_MASK=6 -DSERIAL_BUFFER_SIZE=255 -DADC_SLEEP_MODEOFF

include ../../../../build/avr/targets.mk

dist:
	mkdir -p drummaster/src
	mkdir -p drummaster/lib
	mkdir -p drummaster/build
	cp drummaster.c drummaster/src
	cp drummaster.hex drummaster/src
	cp Makefile drummaster/src
	cp ../build/* drummaster/build
	cp -R ../lib/serial drummaster/lib
	cp -R ../lib/analog drummaster/lib
	cp -R ../lib/timer drummaster/lib
	tar -czvf drummaster.tgz drummaster/*
	rm -rf drummaster

