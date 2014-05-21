#!/bin/bash

avrdude -F -V -c usbtiny -p atmega328p -U efuse:w:0x05:m -U hfuse:w:0xDE:m -U lfuse:w:0xF7:m
avrdude -V -F -p atmega328p -c usbtiny -U flash:w:optiboot_atmega328.hex
