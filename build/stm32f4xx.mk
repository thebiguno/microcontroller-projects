#
# This should not be included in your project-specific Makefile directly.  Rather, include the chip-specific
# makefile, e.g. stm32f411 which in turn includes this one.
#######################################################

# directory to build in
ifndef BUILDDIR
	BUILDDIR = $(abspath $(CURDIR)/build)
endif

ifndef PROGRAMMER
	PROGRAMMER=stlink
endif

OS=$(shell uname -s)

ifeq 'nucleo' '$(PROGRAMMER)'
	ifeq 'Linux' '$(OS)'
		NUCLEO_FOLDER=$(shell find /media/* -name NUCLEO | head -1)
	else
		NUCLEO_FOLDER=/Volumes/NUCLEO
	endif
endif

#Default program and setting definitions
ifndef OBJCOPY
	OBJCOPY=arm-none-eabi-objcopy
endif
ifndef OBJDUMP
	OBJDUMP=arm-none-eabi-objdump
endif


CDEFS+= -DARM_MATH_CM4 -D__FPU_PRESENT=1U

#C and C++ flags
CXXFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Wall -Os -flto -ffunction-sections -fdata-sections -fno-builtin -fno-exceptions -g -ffreestanding

#C flags only
CFLAGS += -std=gnu99

#C++ flags only
CPPFLAGS += -std=gnu++11 -fno-rtti

# linker options
LDFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Wall -Os -Wl,--gc-sections -Wl,--relax --specs=rdimon.specs --specs=nano.specs  --specs=nosys.specs -lm -lc

AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size


# automatically create lists of the sources and objects
LS_FILES1 := $(shell find -L "./inc/common" -name '*.s') $(shell find -L "./inc/arm" -name '*.s') $(shell find -L "./inc/stm32f4" -name '*.s' ! -path "./inc/stm32f4/drivers/CMSIS/Device/ST/*")
LS_FILES2 := $(shell find -L "./inc/common" -name '*.S') $(shell find -L "./inc/arm" -name '*.S') $(shell find -L "./inc/stm32f4" -name '*.S' ! -path "./inc/stm32f4/drivers/CMSIS/Device/ST/*")
LC_FILES := $(shell find -L "./inc/common" -name '*.c') $(shell find -L "./inc/arm" -name '*.c') $(shell find -L "./inc/stm32f4" -name '*.c')
LCPP_FILES := $(shell find -L "./inc/common" -name '*.cpp') $(shell find -L "./inc/arm" -name '*.cpp') $(shell find -L "./inc/stm32f4" -name '*.cpp')
S_FILES_1 := $(shell find -L . -name '*.s' ! -path "./inc/*" ! -path "./cubemx/Drivers/*" ! -path "./build/*")
S_FILES_2 := $(shell find -L . -name '*.S' ! -path "./inc/*" ! -path "./cubemx/Drivers/*" ! -path "./build/*")
C_FILES := $(shell find -L . -name '*.c' ! -path "./inc/*" ! -path "./cubemx/Drivers/*" ! -path "./build/*")
CPP_FILES := $(shell find -L . -name '*.cpp' ! -path "./inc/*" ! -path "./cubemx/Drivers/*" ! -path "./build/*")

# include paths for libraries
L_INC := $(foreach lib,$(shell find -L "./inc/common" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/arm" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./inc/stm32f4" -type d), -I$(lib)) $(foreach lib,$(shell find -L "./cubemx" -type d ! -path "./cubemx/Drivers/"), -I$(lib))

SOURCES := $(S_FILES_1:.s=.o) $(S_FILES2:.S=.o) $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LS_FILES1:.s=.o) $(LS_FILES2:.S=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
#$(info $(SOURCES))

all: bin

build: $(PROJECT).elf

bin: $(PROJECT).bin

flash: all
ifeq 'nucleo' '$(PROGRAMMER)'
	@cp $(PROJECT).bin $(NUCLEO_FOLDER)/
else
	st-flash write $(PROJECT).bin 0x8000000
endif


$(BUILDDIR)/%.o: %.s
	@echo "[AS]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CDEFS) $(CXXFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.S
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
	@$(CXX) $(LDFLAGS) -o "$@" $(OBJS)

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
