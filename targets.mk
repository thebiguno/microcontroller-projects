# Default target.
all: clean build size

build: $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex
	rm -f $(PROJECT).out


$(PROJECT).out: $(SOURCES) 
	$(COMPILER) $(CDEFS) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES)


program: $(PROJECT).hex
	$(AVRDUDE) -V -F -p $(MMCU) -P $(AVRDUDE_PORT) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE)  \
		-U flash:w:$(PROJECT).hex 
		
fuse:
	$(AVRDUDE) -V -F -p $(MMCU) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

readfuse: 
	$(AVRDUDE) -V -F -p $(MMCU) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:r:-:h -U hfuse:r:-:h

size: build
	avr-size $(PROJECT).hex

clean:
	rm -f *.o
	rm -f $(PROJECT).hex
