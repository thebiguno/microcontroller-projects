To install the optiboot bootloader onto a ATMega328 (suitable for running on an RBBB among others),
run the program.sh script.  This will set the fuses and install the bootloader itself.  After programming,
be sure to use the 'arduino' programmer in the DigitalCave build scripts (as this will automatically
set the upload baud rate to 115200).  If you are using a different build system, you can use stk500v1
programmer with baud rate set to 115200.

(Note: as of the time of this writing, I have been unable to get auto reset working on the RBBB + ATMega328.
It works fine on the ATMega168 with default bootloader, so I assume it is a bootloader issue, but have
been unable to get it working properly regardless.  More investigation is needed.)
