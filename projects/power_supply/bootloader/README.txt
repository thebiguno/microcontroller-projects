To install the customized LUFA DFU bootloader onto a ATMega32u4 running on the power supply controller,
run the program.sh script.  This will set the fuses and install the bootloader itself.  This boot loader
requires a 16MHz external crystal.  After programming, be sure to use the ‘dfu’ PROGRAMMER in the 
DigitalCave build scripts as this will automatically use dfu-programmer rather than avrdude to upload.

This bootloader is the stock LUFA build (LUFA-140928), with the following modifications:
	 disable the LEDs in ResetHardware(), and toggle the LEDs in the Timer1OVF ISR.
	-In makefile, I changed the clock speed to 16MHz, the chip to atmega32u4, the program size to 32KB, 
	 and the boot loader size to 4KB.

(Alternatively I could have made a whole new board definition for UBBB, and define LEDs and such there, 
but I felt that was overkill.)

—Wyatt Olson, 2014-07-15, modified for power supply on 2015-05-13
