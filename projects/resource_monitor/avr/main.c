#include <avr/io.h>
#include <util/delay.h>

#include "usb_rawhid.h"
#include "lib/Hd44780/Hd44780_Direct.h"

//#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

using namespace digitalcave;

uint8_t rx_buffer[64];

int main (void){
	// set for 16 MHz clock
//	CPU_PRESCALE(0);

	// Initialize the USB, and then wait for the host to set configuration.
	// If the AVR is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured());

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);
	
	Hd44780_Direct display(display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);

	uint8_t c1[8] = {31,31,0,0,0,0,31,31};
	uint8_t c2[8] = {0,0,0,0,0,0,31,31};
	uint8_t c3[8] = {31,31,0,0,0,0,0,0};
	display.setCgramAddress(0x00);
	_delay_ms(64);
	display.setBytes(c1, 8);
	display.setBytes(c2, 8);
	display.setBytes(c3, 8);

	while (1) {
		// if received data, do something with it
		if (usb_rawhid_recv(rx_buffer, 0) == 64) {
			display.setDdramAddress(0x00);
			_delay_ms(64);        //"1234567890123456789012345678901234567890"
			display.setByte(0xff);
			_delay_ms(64);
			display.setByte(0x00);
			_delay_ms(64);
			display.setByte(0x00);
			_delay_ms(64);
			display.setText((char*) &rx_buffer[0], 16);
			display.setDdramAddress(0x40);
			_delay_ms(64);
			display.setByte(0xff);
			_delay_ms(64);
			display.setByte(0x01);
			_delay_ms(64);
			display.setByte(0xff);
			_delay_ms(64);
			display.setText((char*) &rx_buffer[16], 16);
		}
	}
}

