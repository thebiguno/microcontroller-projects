To install the customized LUFA DFU bootloader onto a ATMega32u2 (suitable for running on a UBBB),
run the program.sh script.  This will set the fuses and install the bootloader itself.  This boot loader
requires a 16MHz external crystal and should have a LED on B4.  After programming, be sure to use the 
‘dfu’ AVRDUDE_PROGRAMMER in the DigitalCave build scripts as this will automatically use dfu-programmer 
rather than avrdude to upload.

This bootloader is the stock LUFA build (LUFA-140302), with the following modifications:
	-In BootloaderDFU.c, I enable the LED in SetupHardware(), disable the LED in ResetHardware(), and toggle 
	 the LED in the Timer1OVF ISR.
	-In makefile, I changed the clock speed to 16MHz, the chip to atmega32u2, the program size to 32KB, 
	 and the boot loader size to 4KB.

(Alternatively I could have made a whole new board definition for UBBB, and define LEDs and such there, 
but I felt that was overkill.)

—Wyatt Olson, 2014-07-15