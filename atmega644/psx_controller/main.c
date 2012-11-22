/*
 * PS2 Controller test.  Connect the wires as indicated in the psx_init() function call.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/psx/psx.h"
#include "lib/serial1/serial1.h"

char temp[32];
uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here

	serial1_init(9600, 8, 0, 1);

	psx_init(&PORTB, PINB1, //Data (Brown)
		&PORTB, PINB0, //Clock (Blue)
		&PORTB, PINB2, //Command (Orange)
		&PORTB, PINB3); //Attention (Yellow)

	//Main program loop
	while (1){
		_delay_ms(10);

		psx_read_gamepad();
		
		/*
		serial1_write_s("0x");
		serial1_write_s(itoa(psx_stick(0), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(1), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(2), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(3), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(4), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(5), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(6), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(7), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(8), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(9), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(10), temp, 16));
		serial1_write_s(", 0x");
		serial1_write_s(itoa(psx_stick(11), temp, 16));
		serial1_write_s("\n\r");
		*/
		
		if (psx_button(PSB_SELECT)) serial1_write_s("Select\n\r");
		if (psx_button(PSB_L1)) serial1_write_s("Left 1\n\r");
		if (psx_button(PSB_L2)) serial1_write_s("Left 2\n\r");
		if (psx_button(PSB_L3)) serial1_write_s("Left 3\n\r");
		if (psx_button(PSB_R1)) serial1_write_s("Right 1\n\r");
		if (psx_button(PSB_R2)) serial1_write_s("Right 2\n\r");
		if (psx_button(PSB_R3)) serial1_write_s("Right 3\n\r");
		if (psx_button(PSB_START)) serial1_write_s("Start\n\r");
		if (psx_button(PSB_PAD_UP)) serial1_write_s("Pad Up\n\r");
		if (psx_button(PSB_PAD_LEFT)) serial1_write_s("Pad Left\n\r");
		if (psx_button(PSB_PAD_DOWN)) serial1_write_s("Pad Down\n\r");
		if (psx_button(PSB_PAD_RIGHT)) serial1_write_s("Pad Right\n\r");
		if (psx_button(PSB_TRIANGLE)) serial1_write_s("Triangle\n\r");
		if (psx_button(PSB_CIRCLE)) serial1_write_s("Circle\n\r");
		if (psx_button(PSB_CROSS)) serial1_write_s("Cross\n\r");
		if (psx_button(PSB_SQUARE)) serial1_write_s("Square\n\r");

		if (psx_stick(PSS_LX) != last_LX){
			last_LX = psx_stick(PSS_LX);
			serial1_write_s("LX: ");
			serial1_write_s(itoa(last_LX, temp, 16));
			serial1_write_s("\n\r");
		}
		if (psx_stick(PSS_LY) != last_LY){
			last_LY = psx_stick(PSS_LY);
			serial1_write_s("LY: ");
			serial1_write_s(itoa(last_LY, temp, 16));
			serial1_write_s("\n\r");
		}
		if (psx_stick(PSS_RX) != last_RX){
			last_RX = psx_stick(PSS_RX);
			serial1_write_s("RX: ");
			serial1_write_s(itoa(last_RX, temp, 16));
			serial1_write_s("\n\r");
		}
		if (psx_stick(PSS_RY) != last_RY){
			last_RY = psx_stick(PSS_RY);
			serial1_write_s("RY: ");
			serial1_write_s(itoa(last_RY, temp, 16));
			serial1_write_s("\n\r");
		}
	}
}
