#!/bin/bash

avrdude -V -F -p atmega32u2 -c usbtiny -U flash:w:BootloaderDFU.hex
avrdude -F -V -c usbtiny -p atmega32u2 -U efuse:w:0xF3:m -U hfuse:w:0xD9:m -U lfuse:w:0xDE:m

