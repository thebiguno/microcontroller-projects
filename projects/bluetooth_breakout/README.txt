How to program the HC-06 boards (which is apparently the one which I have, or some version like it) using this breakout board and a FTDI cable.

1) Physical connections
	a) Solder to the breakout board such that pin 1 on the module is closest to the 3v3 side, pin 13 is closest to GND, etc.  (Note that this is not the order of the actual pins; TX is pin 1, RX is pin 2, etc.  However we wanted this module to be in the same form factor as the XBee, thus 10 pins with VCC at one side and GND at the other, etc).  Truth be told, only pins 1, 2, 12, and 13 need to be soldered, but for stability it is best to just solder everything.
	b) Solder KEY pin to the pad on the back.  For my module, KEY is pin 34 (the one directly opposite VCC).  This can depend on the module in question... most HC-06 modules appear to have KEY as 26, while HC-05 modules appear to have key as 34.  I don't know if I just have a weird module with non-standard firmware or what, but for me KEY is 34 (even though other parts of the firmware appear to be closer to HC-06).  Yay for cheap eBay junk!
	c) Solder pins onto the 10 pin interface header and the 2 pin programming jumper.
2) Software
	a) Start by putting a jumper on the programming header.  This connects KEY to VCC, putting the module in programming mode.
	b) Connect RX / TX on the FTDI cable to the TX / RX on the board (using voltage dividers, zeners, whatever as needed to keep voltages at the proper level).
	c) In a serial terminal (Arduino's terminal works well), set to BAUD=9600 with no CR/LF.  (Note that other modules may require BAUD=38400 with LF+CR... again, this seems to be a HC-05 vs HC-06 thing, but my module appears to have properties of both of these).
	d) Type "AT" (no quotes) to verify that the module is responding.  You should see "OK" echoed back.
	e) You can now use various AT commands to set baud rate, Bluetooth name, etc.  Google "HC-05 Programming" (or HC-06) for details.  See for instance http://www.puntoflotante.net/BOLT-SYSTEM-BLUETOOTH-AT-COMMANDS.htm for an example of what this will look like.

That's it!  Enjoy your Bluetooth serial hacking!
