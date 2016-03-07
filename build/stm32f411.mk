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
ifndef OBJCOPY
	OBJCOPY=arm-none-eabi-objcopy
endif
ifndef OBJDUMP
	OBJDUMP=arm-none-eabi-objdump
endif


CDEFS+=-DF_CPU=$(F_CPU)

#C and C++ flags
CPPFLAGS += -c -Wall -mcpu=cortex-m4 -mlittle-endian -mthumb -Os -DSTM32F411xE -DARM_MATH_CM4 -flto -ffunction-sections -fdata-sections -fno-builtin -fno-exceptions -g -ffreestanding 

#C flags only
CFLAGS += -std=gnu99

#C++ flags only
CXXFLAGS += -std=gnu++11 -fno-rtti

# linker options
LDFLAGS +=  -mcpu=cortex-m4 -Os -mlittle-endian -mthumb -DSTM32F411xE -TTrueSTUDIO/$(PROJECT)/STM32F411RE_FLASH.ld -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs -Wl,--start-group -lgcc -lc -Wl,--end-group

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size


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
			AVRDUDE_PORT=/dev/ttyUSB0
		else
			AVRDUDE_PORT=/dev/tty.usbserial*
		endif
	endif

	ifeq 'Linux' '$(OS)'
		AVRDUDE_PREP_COMMANDS=stty -F $(AVRDUDE_PORT) hupcl
	endif
	AVRDUDE_ARGS += -P $(AVRDUDE_PORT) -b 115200
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
LC_FILES := $(shell find -L "./inc/common" -name '*.c') $(shell find -L "./inc/arm" -name '*.c') $(shell find -L "./Drivers/STM32F4xx_HAL_Driver/Src/" -name '*.c')
LCPP_FILES := $(shell find -L "./inc/common" -name '*.cpp') $(shell find -L "./inc/arm" -name '*.cpp') $(shell find -L "./Drivers/STM32F4xx_HAL_Driver/Src/" -name '*.cpp')
S_FILES := $(shell find -L . -name '*.S' ! -path "./inc/*" ! -path "./Drivers/*" ! -path "./build/*")
C_FILES := $(shell find -L . -name '*.c' ! -path "./inc/*" ! -path "./Drivers/*" ! -path "./build/*")
CPP_FILES := $(shell find -L . -name '*.cpp' ! -path "./inc/*" ! -path "./Drivers/*" ! -path "./build/*")

# include paths for libraries
L_INC := $(foreach lib,$(shell find -L "./inc/common" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/arm" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./Inc" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./Drivers" -type d), -I$(lib))

SOURCES := $(S_FILES:.S=.o) $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
#$(info $(SOURCES))

all: bin

build: $(PROJECT).elf

bin: $(PROJECT).bin

flash: all
ifeq 'dfu' '$(PROGRAMMER)'
	$(DFU) $(MMCU) erase
	$(DFU) $(MMCU) flash $(PROJECT).bin
else
	$(AVRDUDE_PREP_COMMANDS)
	$(AVRDUDE) -F -p $(MMCU) -c $(PROGRAMMER) \
		$(AVRDUDE_ARGS) $(AVRDUDE_SPEED)\
		-U flash:w:$(PROJECT).bin 
endif

$(BUILDDIR)/%.o: %.S
	@echo "[AS]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	$(CC) $(CDEFS) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	$(CXX) $(CDEFS) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(PROJECT).elf: $(OBJS)
	@echo "[LD]\t$@"
	$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

$(PROJECT).asm: $(PROJECT).elf
	@$(OBJDUMP) -C -d $(PROJECT).elf


%.bin: %.elf
	@echo "[HEX]\t$@"
	@$(SIZE) -d "$<"
	@$(OBJCOPY) -O binary -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(PROJECT).elf" "$(PROJECT).bin"

