/*
 * PS2 Controller test.  Connect the wires as indicated in the psx_init() function call.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/psx/psx.h"
#include "lib/serial/serial.h"

char temp[32];
uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);

	psx_init(&PORTA, PINA2, //Data (Brown)
		&PORTA, PINA3, //Clock (Blue)
		&PORTA, PINA1, //Command (Orange)
		&PORTA, PINA0); //Attention (Yellow)

	//Main program loop
	while (1){
		_delay_ms(10);

		psx_read_gamepad();
		
		/*
		serial_write_s("0x");
		serial_write_s(itoa(psx_stick(0), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(1), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(2), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(3), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(4), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(5), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(6), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(7), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(8), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(9), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(10), temp, 16));
		serial_write_s(", 0x");
		serial_write_s(itoa(psx_stick(11), temp, 16));
		serial_write_s("\n\r");
		*/
		
		if (psx_button(PSB_SELECT)) serial_write_s("Select\n\r");
		if (psx_button(PSB_L1)) serial_write_s("Left 1\n\r");
		if (psx_button(PSB_L2)) serial_write_s("Left 2\n\r");
		if (psx_button(PSB_L3)) serial_write_s("Left 3\n\r");
		if (psx_button(PSB_R1)) serial_write_s("Right 1\n\r");
		if (psx_button(PSB_R2)) serial_write_s("Right 2\n\r");
		if (psx_button(PSB_R3)) serial_write_s("Right 3\n\r");
		if (psx_button(PSB_START)) serial_write_s("Start\n\r");
		if (psx_button(PSB_PAD_UP)) serial_write_s("Pad Up\n\r");
		if (psx_button(PSB_PAD_LEFT)) serial_write_s("Pad Left\n\r");
		if (psx_button(PSB_PAD_DOWN)) serial_write_s("Pad Down\n\r");
		if (psx_button(PSB_PAD_RIGHT)) serial_write_s("Pad Right\n\r");
		if (psx_button(PSB_TRIANGLE)) serial_write_s("Triangle\n\r");
		if (psx_button(PSB_CIRCLE)) serial_write_s("Circle\n\r");
		if (psx_button(PSB_CROSS)) serial_write_s("Cross\n\r");
		if (psx_button(PSB_SQUARE)) serial_write_s("Square\n\r");

		if (psx_stick(PSS_LX) != last_LX){
			last_LX = psx_stick(PSS_LX);
			serial_write_s("LX: ");
			serial_write_s(itoa(last_LX, temp, 16));
			serial_write_s("\n\r");
		}
		if (psx_stick(PSS_LY) != last_LY){
			last_LY = psx_stick(PSS_LY);
			serial_write_s("LY: ");
			serial_write_s(itoa(last_LY, temp, 16));
			serial_write_s("\n\r");
		}
		if (psx_stick(PSS_RX) != last_RX){
			last_RX = psx_stick(PSS_RX);
			serial_write_s("RX: ");
			serial_write_s(itoa(last_RX, temp, 16));
			serial_write_s("\n\r");
		}
		if (psx_stick(PSS_RY) != last_RY){
			last_RY = psx_stick(PSS_RY);
			serial_write_s("RY: ");
			serial_write_s(itoa(last_RY, temp, 16));
			serial_write_s("\n\r");
		}
	}
}
