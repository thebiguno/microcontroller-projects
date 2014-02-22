F_CPU = 12000000
DEVICE = atmega1284
PROGRAMMER = usbtiny

#atmega 328p 4kB (2048 words)
#BOOTLOADER_ADDRESS = 0x3800
#LFUSE = 0xFF
#HFUSE = 0xD8
#EFUSE = 0xFF

#atmega 644 / 644p / 1284 / 1284p 4kB (2048 words)
BOOTLOADER_ADDRESS = 0xFC00
LFUSE = 0xFF
HFUSE = 0xDA
EFUSE = 0xFF


# 0xFF for no restrictions
LOCK = 0xFF

CC = avr-gcc

# Options:
DEFINES = #-DDEBUG_LEVEL=2
CFLAGS = -Wall -Os -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) $(DEFINES)
LDFLAGS = -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)

OBJECTS =  usbdrv/usbdrvasm.o main.o #usbdrv/oddebug.o

# symbolic targets:
all: main.hex

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

clean:
	rm -f main.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

main.bin:	$(OBJECTS)
	$(CC) $(CFLAGS) -o main.bin $(OBJECTS) $(LDFLAGS)

main.hex:	main.bin
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avr-size main.hex

disasm:	main.bin
	avr-objdump -d main.bin

writefuse:
	avrdude -c $(PROGRAMMER) -p $(DEVICE) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

readfuse:
	avrdude -c $(PROGRAMMER) -p $(DEVICE) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

program:	all
	avrdude -V -c $(PROGRAMMER) -p $(DEVICE) -U flash:w:$main.hex

cpp:
	$(CC) $(CFLAGS) -E main.c

# Special rules for generating hex files for various devices and clock speeds
ALLHEXFILES = hexfiles/mega8_12mhz.hex hexfiles/mega8_15mhz.hex hexfiles/mega8_16mhz.hex \
	hexfiles/mega88_12mhz.hex hexfiles/mega88_15mhz.hex hexfiles/mega88_16mhz.hex hexfiles/mega88_20mhz.hex\
	hexfiles/mega168_12mhz.hex hexfiles/mega168_15mhz.hex hexfiles/mega168_16mhz.hex hexfiles/mega168_20mhz.hex

allhexfiles: $(ALLHEXFILES)
	$(MAKE) clean
	avr-size hexfiles/*.hex
	avr-size hexfiles/*.hex >> results.txt

$(ALLHEXFILES):
	@[ -d hexfiles ] || mkdir hexfiles
	@device=`echo $@ | sed -e 's|.*/mega||g' -e 's|_.*||g'`; \
	clock=`echo $@ | sed -e 's|.*_||g' -e 's|mhz.*||g'`; \
	addr=`echo $$device | sed -e 's/\([0-9]\)8/\1/g' | awk '{printf("%x", ($$1 - 2) * 1024)}'`; \
	echo "### Make with F_CPU=$${clock}000000 DEVICE=atmega$$device BOOTLOADER_ADDRESS=$$addr"; \
	$(MAKE) clean; \
	$(MAKE) main.hex F_CPU=$${clock}000000 DEVICE=atmega$$device BOOTLOADER_ADDRESS=$$addr DEFINES=-DUSE_AUTOCONFIG=1
	mv main.hex $@
