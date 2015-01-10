#!/bin/bash

avrdude -V -p atmega32u2 -c usbtiny -e
avrdude -V -p atmega32u2 -c usbtiny -U flash:w:BootloaderDFU.hex
avrdude -V -p atmega32u2 -c usbtiny -U efuse:w:0xF3:m -U hfuse:w:0xD9:m -U lfuse:w:0xDE:m

