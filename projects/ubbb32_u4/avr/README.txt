To install the customized LUFA DFU bootloader onto a ATMega32u2 (suitable for running on a UBBB),
run the program.sh script.  This will set the fuses and install the bootloader itself.  This boot loader
requires a 16MHz external crystal and should have a LED on B4.  After programming, be sure to use the 
‘dfu’ PROGRAMMER in the DigitalCave build scripts as this will automatically use dfu-programmer 
rather than avrdude to upload.

This bootloader is the stock LUFA build (LUFA-140302), with the following modifications:
	-In BootloaderDFU.c, I enable the blue (B4) and red (B6) LEDs in SetupHardware() and put B6 high, 
	 disable the LEDs in ResetHardware(), and toggle the LEDs in the Timer1OVF ISR.
	-In makefile, I changed the clock speed to 16MHz, the chip to atmega32u4, the program size to 32KB, 
	 and the boot loader size to 4KB.

(Alternatively I could have made a whole new board definition for UBBB, and define LEDs and such there, 
but I felt that was overkill.)

—Wyatt Olson, 2014-07-15, modified for u4 on 2015-01-09
