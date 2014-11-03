#include <avr/io.h>
#include <util/delay.h>

#include "lib/Hd44780/Hd44780_Direct.h"
#include "lib/usb/rawhid/usb_rawhid.h"

using namespace digitalcave;

uint8_t rx_buffer[64];

int main (void){
	// Initialize the USB, and then wait for the host to set configuration.
	// If the AVR is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured());

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);
	
	Hd44780_Direct display(display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);

	uint8_t custom[64] = {
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,			// 0 (Use 0xFE)
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1f,			// 1
		0x0,0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,			// 2
		0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,			// 3
		0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,		// 4
		0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,		// 5
		0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,		// 6
		0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f		// 7
													// 8 (Use 0xFF)
	};
	display.setCgramAddress(0x00);
	_delay_ms(64);
	display.setBytes(custom, 64);

	while (1) {
		// if received data, do something with it
		if (usb_rawhid_recv(rx_buffer, 0) >= 32) {
			display.setDdramAddress(0x00);
			_delay_ms(64);
			display.setText((char*) &rx_buffer[0], 16);
			display.setDdramAddress(0x40);
			display.setText((char*) &rx_buffer[16], 16);

		}
	}
}

