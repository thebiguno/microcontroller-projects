#Project name
PROJECT=skeleton

#Source files (.c, .cpp, and .s)
CSOURCES=main.c lib/stm32f4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
CPPSOURCES=
SSOURCES=lib/stm32f4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s

#Utilities
CC=arm-none-eabi-gcc
OPENOCD=openocd
OBJCOPY=arm-none-eabi-objcopy

#Utility config
OPENOCD_CONFIG=board/st_nucleo_f411re.cfg

#Target objects
OBJECTS=$(CSOURCES:.c=.c.o) $(CPPSOURCES:.cpp=.cpp.o) $(SSOURCES:.s=.s.o)

#Compiler config
INCLUDES+=-Ilib/stm32f4/Drivers/CMSIS/Device/ST/STM32F4xx/Include -Ilib/stm32f4/Drivers/CMSIS/Include
CFLAGS+=-Wall -mcpu=cortex-m4 -mlittle-endian -mthumb -DSTM32F411xE -Os -c
LDFLAGS+=-Wl,--gc-sections -Tlib/stm32f4/Projects/STM32F411RE-Nucleo/Templates/TrueSTUDIO/STM32F4xx-Nucleo/STM32F411RE_FLASH.ld

#Targets
all: clean $(PROJECT).hex

flash: all
	$(OPENOCD) -f $(OPENOCD_CONFIG) -c "program $(PROJECT).hex verify reset"
	
$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -Oihex $(PROJECT).elf $(PROJECT).hex

$(PROJECT).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.c.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

%.cpp.o: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

%.s.o: %.s
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(PROJECT).elf
	rm -f $(PROJECT).hex