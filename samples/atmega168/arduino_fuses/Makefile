include ../../build/variables.mk

MMCU=atmega168
F_CPU=20000000
AVRDUDE_PROGRAMMER=usbtiny

LFUSE=0xFF
HFUSE=0xDE
EFUSE=0x00

# You can also define anything here and it will override
# the definitions in variables.mk

fuse:
	$(AVRDUDE) -V -F -p $(MMCU) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

readfuse: 
	$(AVRDUDE) -V -F -p $(MMCU) \
		-c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE) \
		-U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

