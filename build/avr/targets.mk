ifndef COMPILER 
	COMPILER=avr-gcc
endif
ifeq 'avr-gcc' '$(COMPILER)'
	CFLAGS += -std=gnu99
endif
ifeq 'avr-g++' '$(COMPILER)'
	CFLAGS += -std=gnu++11
endif

ifeq 'stk500v1' '$(AVRDUDE_PROGRAMMER)'
	ifeq ($(OS),Linux)
		AVRDUDE_PREP_COMMANDS=stty -F $(AVRDUDE_PORT) hupcl
	endif
	AVRDUDE_ARGS += -P $(AVRDUDE_PORT) -b 19200
endif
#We use the arduino programmer to mean 'Arduino Uno with Optiboot'; this is stk500v1 compatible at 115200 baud.
ifeq 'arduino' '$(AVRDUDE_PROGRAMMER)'
	ifeq ($(OS),Linux)
		AVRDUDE_PREP_COMMANDS=stty -F $(AVRDUDE_PORT) hupcl
	endif
	AVRDUDE_ARGS += -P $(AVRDUDE_PORT) -b 115200
endif

ifeq 'usbtiny' '$(AVRDUDE_PROGRAMMER)'
	AVRDUDE_SPEED = -B 1
endif

#If an EFUSE variable has been set, we program the extended fuses too
ifeq '' '$(EFUSE)'
	EXTENDED_FUSE_WRITE=
else
	EXTENDED_FUSE_WRITE=-U efuse:w:$(EFUSE):m
endif

# Default target.

all: clean build

build: $(PROJECT).hex

assembly: $(PROJECT).asm

readelf: $(PROJECT).elf
	$(READELF) -a $(PROJECT).elf

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).elf $(PROJECT).hex
	$(AVRSIZE) -d -C --mcu=$(MMCU) $(PROJECT).elf

$(PROJECT).asm: $(PROJECT).elf
	$(OBJDUMP) -C -d $(PROJECT).elf

$(PROJECT).elf: $(SOURCES) 
	$(COMPILER) $(CDEFS) $(CFLAGS) -I./ -o $(PROJECT).elf $(SOURCES) $(CLIBS)


program: all
ifeq 'dfu' '$(AVRDUDE_PROGRAMMER)'
	$(DFU) $(MMCU) erase
	$(DFU) $(MMCU) flash $(PROJECT).hex
else
	$(AVRDUDE_PREP_COMMANDS)
	$(AVRDUDE) -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) $(AVRDUDE_SPEED)\
		-U flash:w:$(PROJECT).hex 
endif

fuse:
ifeq 'dfu' '$(AVRDUDE_PROGRAMMER)'
	echo "Cannot set fuses in DFU Programmer mode"
else
	$(AVRDUDE) -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m $(EXTENDED_FUSE_WRITE)
endif

readfuse: 
ifeq 'dfu' '$(AVRDUDE_PROGRAMMER)'
	echo "Cannot set fuses in DFU Programmer mode"
else
	$(AVRDUDE) -V -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
endif

readeeprom: 
ifeq 'dfu' '$(AVRDUDE_PROGRAMMER)'
	$(DFU) $(MMCU) dump-eeprom --quiet | hexdump
else
	$(AVRDUDE) -V -F -p $(MMCU) -c $(AVRDUDE_PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U eeprom:r:-:h
endif

clean:
	rm -f *.o
	rm -f $(PROJECT).hex
	rm -f $(PROJECT).elf
