#
# Include this file from your project-specific Makefile after setting up some key variables.
# At a minimum, you must include the following:
# 
# PROJECT=MyProjectName
#
# Additional supported options include:
# CDEFS (project specific defines, etc)
# PROGRAMMER (st-flash, nucleo, etc)
#######################################################

CXXFLAGS +=  -DSTM32F411xE

LDFLAGS += -DSTM32F411xE -Tcubemx/TrueSTUDIO/cubemx/STM32F411RE_FLASH.ld 

STARTUP_FILES := $(shell find -L "./inc/stm32f4/drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc" -name 'startup_stm32f411xe.s')

SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)stm32f4xx.mk