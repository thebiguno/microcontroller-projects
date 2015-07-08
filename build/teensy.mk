#
# Include this file from your project-specific Makefile after setting up some key variables.
# At a minimum, you must include the following:
# 
# PROJECT=MyProjectName
# TEENSY=31 or 30 or LC
# F_CPU=24000000, 48000000, 96000000, etc
# OPTIONS=???? (defaults to -DUSB_SERIAL -DLAYOUT_US_ENGLISH)
#
# This build script relies on the build tools from Arduino with the Teensyduino addons.  Please ensure
# that you have this installed in the proper location.
#
# You also need to ensure that the required libraries are sym linked / copied to the 'lib' folder.  Most
# projects will want both the 'common' and the 'teensy' libraries.
#######################################################

# directory to build in
BUILDDIR = $(abspath $(CURDIR)/build)

# configurable options
ifndef OPTIONS
	OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH
endif

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(CURDIR)/tools

ifeq ($(OS),Windows_NT)
	$(error What is Win Dose?)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		TOOLSPATH = /Applications/Arduino.app/Contents/Java/hardware/tools/
	else
		TOOLSPATH = /opt/arduino/hardware/tools/
	endif
endif

# path location for Arduino libraries
LIBRARYPATH = lib

# path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)/arm/bin

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -mthumb -ffunction-sections -fdata-sections -nostdlib -MMD $(OPTIONS) -DTEENSYDUINO=124 -DF_CPU=$(F_CPU) -Isrc

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# path location for Teensy 3 core
COREPATH = $(LIBRARYPATH)/teensy/Core/

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mthumb

# additional libraries to link
LIBS = -lm

# compiler options specific to teensy version
ifeq ($(TEENSY), 30)
	CPPFLAGS += -D__MK20DX128__ -mcpu=cortex-m4
	LDSCRIPT = $(COREPATH)/mk20dx128.ld
	LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
else
	ifeq ($(TEENSY), 31)
		CPPFLAGS += -D__MK20DX256__ -mcpu=cortex-m4
		LDSCRIPT = $(COREPATH)/mk20dx256.ld
		LDFLAGS += -mcpu=cortex-m4 -T$(LDSCRIPT)
	else
		ifeq ($(TEENSY), LC)
			CPPFLAGS += -D__MKL26Z64__ -mcpu=cortex-m0plus
			LDSCRIPT = $(COREPATH)/mkl26z64.ld
			LDFLAGS += -mcpu=cortex-m0plus -T$(LDSCRIPT)
			LIBS += -larm_cortexM0l_math
		else
			$(error Invalid setting for TEENSY)
		endif
	endif
endif

# set arduino define if given
ifdef ARDUINO
	CPPFLAGS += -DARDUINO=$(ARDUINO)
else
	CPPFLAGS += -DUSING_MAKEFILE
endif

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size

# automatically create lists of the sources and objects
LC_FILES := $(shell find -L $(LIBRARYPATH) -name '*.c')
LCPP_FILES := $(shell find -L $(LIBRARYPATH) -name '*.cpp')
C_FILES := $(shell find . -name '*.c' ! -path "./lib/*" ! -path "./build/*")
CPP_FILES := $(shell find . -name '*.cpp' ! -path "./lib/*" ! -path "./build/*")

# include paths for libraries
L_INC := $(foreach lib,$(shell find -L $(LIBRARYPATH)/ -type d), -I$(lib))

SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))
$(info $(L_INC))

all: hex

build: $(PROJECT).elf

hex: $(PROJECT).hex

post_compile: $(PROJECT).hex
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" -path=$(CURDIR) -tools="$(abspath $(TOOLSPATH))"

reboot:
	@-$(abspath $(TOOLSPATH))/teensy_reboot

flash: post_compile reboot

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -x c++ -include Arduino.h -c "$<"

$(PROJECT).elf: $(OBJS) $(LDSCRIPT)
	@echo "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

%.hex: %.elf
	@echo "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(PROJECT).elf" "$(PROJECT).hex"
