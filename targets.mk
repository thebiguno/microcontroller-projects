ifndef COMPILER 
	COMPILER=avr-gcc
endif
ifeq 'avr-gcc' '$(COMPILER)'
	CFLAGS += -std=gnu99
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
	$(AVRDUDE) -V -F -p $(MMCU) -P $(AVRDUDE_PORT) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE)  \
		-U flash:w:$(PROJECT).hex 
		
fuse:
	$(AVRDUDE) -V -F -p $(MMCU) -P $(AVRDUDE_PORT) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

readfuse: 
	$(AVRDUDE) -V -F -p $(MMCU) -P $(AVRDUDE_PORT) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:r:-:h -U hfuse:r:-:h

clean:
	rm -f *.o
	rm -f $(PROJECT).hex
