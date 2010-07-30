include ../../../build/variables.mk

PROJECT=FilesystemTest
MMCU=atmega644
F_CPU=20000000
SOURCES=lib/sd-card/main.c lib/sd-card/byteordering.c lib/sd-card/fat.c lib/sd-card/partition.c lib/sd-card/sd_raw.c lib/sd-card/uart.c

# You can also define anything here and it will override 
# the definitions in variables.mk

AVRDUDE_PROGRAMMER=usbtiny

include ../../../build/targets.mk

