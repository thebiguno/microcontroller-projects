ifndef COMPILER 
	COMPILER=avr-gcc
endif
ifeq 'avr-gcc' '$(COMPILER)'
	CFLAGS += -std=gnu99
endif

ifeq 'stk500v1' '$(AVRDUDE_PROGRAMMER)'
	AVRDUDE_ARGS += -P /dev/usbserial*
endif

# Default target.

all: clean build

build: $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex
	avr-size -d -C --mcu=$(MMCU) $(PROJECT).out
	@rm -f $(PROJECT).out


$(PROJECT).out: $(SOURCES) 
	$(COMPILER) $(CDEFS) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES) $(CLIBS)


program: all
	$(AVRDUDE) -V -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U flash:w:$(PROJECT).hex 
		
fuse:
	$(AVRDUDE) -V -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

readfuse: 
	$(AVRDUDE) -V -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:r:-:h -U hfuse:r:-:h

clean:
	rm -f *.o
	rm -f $(PROJECT).hex
