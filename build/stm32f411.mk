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


CDEFS+=

#C and C++ flags
CXXFLAGS += -Wall -mcpu=cortex-m4 -mlittle-endian -mthumb -Os -DSTM32F411xE -DARM_MATH_CM4 -flto -ffunction-sections -fdata-sections -fno-builtin -fno-exceptions -g -ffreestanding 

#C flags only
CFLAGS += -std=gnu99

#C++ flags only
CPPFLAGS += -std=gnu++11 -fno-rtti

# linker options
LDFLAGS += -mcpu=cortex-m4 -Os -mlittle-endian -mthumb -DSTM32F411xE -Tlinker/STM32F411RE_FLASH.ld -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs -Wl,--start-group -lgcc -lc -Wl,--end-group

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size


# automatically create lists of the sources and objects
LS_FILES := $(shell find -L "./inc/common" -iname '*.s') $(shell find -L "./inc/arm" -iname '*.s') $(shell find -L "./inc/stm32f4" -iname '*.s')
LC_FILES := $(shell find -L "./inc/common" -iname '*.c') $(shell find -L "./inc/arm" -iname '*.c') $(shell find -L "./inc/stm32f4" -iname '*.c')
LCPP_FILES := $(shell find -L "./inc/common" -iname '*.cpp') $(shell find -L "./inc/arm" -iname '*.cpp') $(shell find -L "./inc/stm32f4" -iname '*.cpp')
S_FILES := $(shell find -L . -name '*.S' ! -path "./inc/*" ! -path "./build/*") $(shell find -L . -name '*.s' ! -path "./inc/*" ! -path "./build/*")
C_FILES := $(shell find -L . -name '*.c' ! -path "./inc/*" ! -path "./build/*")
CPP_FILES := $(shell find -L . -name '*.cpp' ! -path "./inc/*" ! -path "./build/*")

# include paths for libraries
L_INC := $(foreach lib,$(shell find -L "./inc/common" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/arm" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/stm32f4" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./config" -type d), -I$(lib))

SOURCES := $(S_FILES:.S=.o) $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LS_FILES:.s=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
#$(info $(SOURCES))

all: bin

build: $(PROJECT).elf

bin: $(PROJECT).bin

flash: all
	st-flash --reset write $(PROJECT).bin 0x8000000 

$(BUILDDIR)/%.o: %.s
	@echo "[AS]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CXXFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CXXFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CDEFS) $(CXXFLAGS) $(CPPFLAGS) $(L_INC) -o "$@" -c "$<"

$(PROJECT).elf: $(OBJS)
	@echo "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS)

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

