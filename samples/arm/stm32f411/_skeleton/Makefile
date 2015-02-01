#Project name
PROJECT=skeleton

#Source files (.c, .cpp, and .s)
CSOURCES=main.c stm32f4xx_it.c stm32f4xx_nucleo.c
CPPSOURCES=
SSOURCES=

CSOURCES+=lib/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c lib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
SSOURCES+=lib/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s

#Utilities
CC=arm-none-eabi-gcc
OPENOCD=openocd
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

#Utility config
OPENOCD_CONFIG=board/st_nucleo_f411re.cfg

#Target objects
OBJECTS+=$(CSOURCES:.c=.c.o) $(CPPSOURCES:.cpp=.cpp.o) $(SSOURCES:.s=.s.o)

#Compiler config
MCUFLAGS=-mthumb -mthumb-interwork -mcpu=cortex-m4
INCLUDES+=-I. -Ilib/stm32cubef4/Drivers/CMSIS/Device/ST/STM32F4xx/Include -Ilib/stm32cubef4/Drivers/CMSIS/Include -Ilib/stm32cubef4/Drivers/STM32F4xx_HAL_Driver/Inc
CFLAGS+=-Wall $(MCUFLAGS) -mlittle-endian -std=gnu99 -DSTM32F411xE -Os -c
LDFLAGS+=-Wl,--gc-sections $(MCUFLAGS) -Tlib/stm32cubef4/Projects/STM32F411RE-Nucleo/Templates/TrueSTUDIO/STM32F4xx-Nucleo/STM32F411RE_FLASH.ld

#Targets
all: clean $(PROJECT).hex

flash: all
	$(OPENOCD) -f $(OPENOCD_CONFIG) -c "program $(PROJECT).hex verify reset"

bin: all
	$(OBJCOPY) -I ihex $(PROJECT).hex $(PROJECT).bin
	
$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex $(PROJECT).elf $(PROJECT).hex
	$(SIZE) $(PROJECT).elf

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