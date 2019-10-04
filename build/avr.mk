#
# Include this file from your project-specific Makefile after setting up some key variables.
# At a minimum, you must include the following:
#
# PROJECT=MyProjectName
# MMCU=atmega168
# F_CPU=20000000
#
# Additional supported options include:
# CDEFS (project specific defines, etc)
# PROGRAMMER (usbtinyisp, avrispmkII, arduino, atk500v1, dfu, etc)
# HFUSE (High fuse byte)
# LFUSE (Low fuse byte)
# EFUSE (Extended fuse byte)
# ... etc
#
# You also need to ensure that the required libraries are sym linked / copied to the 'lib' folder.  Most
# projects will want both the 'common' and the 'avr' libraries.
#######################################################

# directory to build in
ifndef BUILDDIR
	BUILDDIR = $(abspath $(CURDIR)/build)
endif


OS=$(shell uname -s)

#Default program and setting definitions
ifndef DFU
	DFU=dfu-programmer
endif
ifndef AVRDUDE
	AVRDUDE=avrdude
endif
ifndef PROGRAMMER
	PROGRAMMER=dfu
endif
ifndef OBJCOPY
	OBJCOPY=avr-objcopy
endif
ifndef OBJDUMP
	OBJDUMP=avr-objdump
endif
ifndef READELF
	READELF=avr-readelf
endif
ifndef AVRSIZE
	AVRSIZE=avr-size
endif


CDEFS+=-DF_CPU=$(F_CPU)

#C and C++ flags
CPPFLAGS += -mmcu=$(MMCU) -pedantic -g -Os -Wall -fdata-sections -ffunction-sections -ffreestanding

#C flags only
CFLAGS += -std=gnu99

#C++ flags only
CXXFLAGS += -std=gnu++11

# linker options
LDFLAGS += -mmcu=$(MMCU) -Os -Wl,--gc-sections -Wl,--relax

AS = avr-as
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size


ifeq 'stk500v1' '$(PROGRAMMER)'
	ifeq 'Linux' '$(OS)'
		AVRDUDE_PREP_COMMANDS=stty -F $(AVRDUDE_PORT) hupcl
	endif
	ifndef AVRDUDE_PORT
		ifeq 'Linux' '$(OS)'
			AVRDUDE_PORT=/dev/ttyUSB0
		else
			AVRDUDE_PORT=/dev/tty.usbserial*
		endif
	endif
	AVRDUDE_ARGS += -P $(AVRDUDE_PORT) -b 19200
endif
#We use the arduino programmer to mean 'Arduino Uno with Optiboot'; this is stk500v1 compatible at 115200 baud.
ifeq 'arduino' '$(PROGRAMMER)'
	ifndef AVRDUDE_PORT
		ifeq 'Linux' '$(OS)'
			AVRDUDE_PORT=/dev/ttyACM0
		else
			AVRDUDE_PORT=/dev/tty.usbserial*
		endif
	endif

	ifeq 'Linux' '$(OS)'
		AVRDUDE_PREP_COMMANDS=stty -F $(AVRDUDE_PORT) hupcl
	endif
	AVRDUDE_ARGS += -P $(AVRDUDE_PORT) -b 115200
endif

#This is the Pololu arduino bootloader; use avr109 as per http://pololu.com/docs/0J61/6.3
ifeq 'avr109' '$(PROGRAMMER)'
	ifndef AVRDUDE_PORT
		ifeq 'Linux' '$(OS)'
			AVRDUDE_PORT=/dev/ttyACM0
		else
			AVRDUDE_PORT=/dev/tty.usbserial*
		endif
	endif

	AVRDUDE_ARGS += -P $(AVRDUDE_PORT)
endif

ifeq 'usbtiny' '$(PROGRAMMER)'
	ifndef AVRDUDE_SPEED
		AVRDUDE_SPEED = -B 1
	endif
endif

ifeq 'avrispmkII' '$(PROGRAMMER)'
	AVRDUDE_ARGS += -P usb
endif

#If an EFUSE variable has been set, we program the extended fuses too
ifeq '' '$(EFUSE)'
	EXTENDED_FUSE_WRITE=
else
	EXTENDED_FUSE_WRITE=-U efuse:w:$(EFUSE):m
endif

# automatically create lists of the sources and objects
LC_FILES := $(shell find -L "./inc/common" -name '*.c') $(shell find -L "./inc/avr" -name '*.c')
LCPP_FILES := $(shell find -L "./inc/common" -name '*.cpp') $(shell find -L "./inc/avr" -name '*.cpp')
S_FILES := $(shell find -L . -name '*.S' ! -path "./inc/*" ! -path "./build/*")
C_FILES := $(shell find -L . -name '*.c' ! -path "./inc/*" ! -path "./build/*")
CPP_FILES := $(shell find -L . -name '*.cpp' ! -path "./inc/*" ! -path "./build/*")

# include paths for libraries
L_INC := $(foreach lib,$(shell find -L "./inc/common" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/avr" -type d), -I$(lib))

SOURCES := $(S_FILES:.S=.o) $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
#$(info $(SOURCES))

all: hex

build: $(PROJECT).elf

hex: $(PROJECT).hex

flash: all
ifeq 'dfu' '$(PROGRAMMER)'
	$(DFU) $(MMCU) erase
	$(DFU) $(MMCU) flash $(PROJECT).hex
else
	$(AVRDUDE_PREP_COMMANDS)
	$(AVRDUDE) -F -p $(MMCU) -c $(PROGRAMMER) \
		$(AVRDUDE_ARGS) $(AVRDUDE_SPEED)\
		-U flash:w:$(PROJECT).hex
endif

$(BUILDDIR)/%.o: %.S
	@echo "[AS]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CDEFS) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(PROJECT).elf: $(OBJS)
	@echo "[LD]\t$@"
	@$(CXX) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

$(PROJECT).asm: $(PROJECT).elf
	@$(OBJDUMP) -C -d $(PROJECT).elf


%.hex: %.elf
	@echo "[HEX]\t$@"
	@$(SIZE) -d -C --mcu=$(MMCU) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(PROJECT).elf" "$(PROJECT).hex"

fuse:
ifeq 'dfu' '$(PROGRAMMER)'
	echo "Cannot set fuses in DFU Programmer mode"
else
	$(AVRDUDE) -F -p $(MMCU) -c $(PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m $(EXTENDED_FUSE_WRITE)
endif

start:
ifeq 'dfu' '$(PROGRAMMER)'
	$(DFU) $(MMCU) start
else
	echo "Cannor start in AVR Dude Programmer mode"
endif

readfuse:
ifeq 'dfu' '$(PROGRAMMER)'
	echo "Cannot read fuses in DFU Programmer mode"
else
	$(AVRDUDE) -V -F -p $(MMCU) -c $(PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
endif

readeeprom:
ifeq 'dfu' '$(PROGRAMMER)'
	$(DFU) $(MMCU) dump-eeprom --quiet | hexdump
else
	$(AVRDUDE) -V -F -p $(MMCU) -c $(PROGRAMMER) \
		$(AVRDUDE_ARGS) \
		-U eeprom:r:-:h
endif
