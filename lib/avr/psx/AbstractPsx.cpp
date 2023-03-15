/******************************************************************
*  PS2 controller library for AVR
*
*	Loosly based off of code by Shutter on Arduino Forums, and modified 
*	(added analog stick support) by Bill Porter (http://www.billporter.info/?p=219)
*
*	Converted to generic AVR library without C++ or Arduino references, added 
*	the ability to use any pins for input / output, fixed numerous bugs, actually
*	follow proper PS2 controller protocols, etc by Wyatt Olson (http://digitalcave.ca)
*
*	Thanks to http://store.curiousinventor.com/guides/PS2/#high_level for protocol details.
*
*  Lib version history
*	 ????		0.1		Bill Porter		-Made into Arduino library
*										-Added analog stick support
*	 2010-07	0.2		Wyatt Olson		-Converted to generic AVR-GCC library
*										-Added port / pin initializers (psx_init())
*										-Changed to use uintx_t data types instead of char
*										-Fixed many bugs (including glaring protocol violations)
*										-Code cleanup
*	 2011-04	0.3		Wyatt Olson		-Fixed a number of logic bugs, where the impl did not match spec
*										-Code cleanup (merged methods, fixed comments, etc)
* 
******************************************************************/


#include "AbstractPsx.h"

using namespace digitalcave;

void AbstractPsx::poll() {
	data[0] = 0x01;
	data[1] = 0x42;
	for (uint8_t i = 2; i < 21; i++){
		data[i] = 0x00;
	}
	sendCommand(data, 21);
}

uint16_t AbstractPsx::changed() {
	return buttons_changed;
}

uint16_t AbstractPsx::buttons() {
	uint16_t read = ~(*(uint16_t*)(data + 3)); //Get 2 bytes, comprising data positions 3 and 4.
	buttons_changed = buttons_state ^ read;
	buttons_state = read;
	return buttons_state;
}

uint8_t AbstractPsx::button(uint16_t button) {
	return ((buttons() & button) > 0);
}

uint8_t AbstractPsx::stick(unsigned int stick) {
	return data[stick];
}
