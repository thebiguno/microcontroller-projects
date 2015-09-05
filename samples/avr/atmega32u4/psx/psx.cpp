/*
 * PS2 Controller test.  Connect the wires as indicated in the PSX constructor
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include <PSX_AVR.h>
#include <Serial_AVR.h>

using namespace digitalcave;

char temp[32];
uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here
	

	//Serial_AVR serial(9600);

	PSX_AVR psx(&PORTD, PORTD2, //Data (Brown)
		&PORTD, PORTD3, //Command (Orange)
		&PORTD, PORTD4, //Clock (Blue)
		&PORTD, PORTD5); //Attention (Yellow)
	
	//Main program loop
	while (1){
		_delay_ms(10);

		psx.poll();
		
		if (psx.stick(PSS_LX) != last_LX){
			last_LX = psx.stick(PSS_LX);
			PORTB ^= _BV(PORTB4);
		}
		
		if (psx.button(PSB_PAD_LEFT)) PORTB |= _BV(PORTB5);
		else PORTB &= ~_BV(PORTB5);


// 		if (psx.button(PSB_SELECT)) serial.write("Select\n\r");
// 		if (psx.button(PSB_L1)) serial.write("Left 1\n\r");
// 		if (psx.button(PSB_L2)) serial.write("Left 2\n\r");
// 		if (psx.button(PSB_L3)) serial.write("Left 3\n\r");
// 		if (psx.button(PSB_R1)) serial.write("Right 1\n\r");
// 		if (psx.button(PSB_R2)) serial.write("Right 2\n\r");
// 		if (psx.button(PSB_R3)) serial.write("Right 3\n\r");
// 		if (psx.button(PSB_START)) serial.write("Start\n\r");
// 		if (psx.button(PSB_PAD_UP)) serial.write("Pad Up\n\r");
// 		if (psx.button(PSB_PAD_LEFT)) serial.write("Pad Left\n\r");
// 		if (psx.button(PSB_PAD_DOWN)) serial.write("Pad Down\n\r");
// 		if (psx.button(PSB_PAD_RIGHT)) serial.write("Pad Right\n\r");
// 		if (psx.button(PSB_TRIANGLE)) serial.write("Triangle\n\r");
// 		if (psx.button(PSB_CIRCLE)) serial.write("Circle\n\r");
// 		if (psx.button(PSB_CROSS)) serial.write("Cross\n\r");
// 		if (psx.button(PSB_SQUARE)) serial.write("Square\n\r");
// 
// 		if (psx.stick(PSS_LX) != last_LX){
// 			last_LX = psx.stick(PSS_LX);
// 			serial.write("LX: ");
// 			serial.write(itoa(last_LX, temp, 16));
// 			serial.write("\n\r");
// 		}
// 		if (psx.stick(PSS_LY) != last_LY){
// 			last_LY = psx.stick(PSS_LY);
// 			serial.write("LY: ");
// 			serial.write(itoa(last_LY, temp, 16));
// 			serial.write("\n\r");
// 		}
// 		if (psx.stick(PSS_RX) != last_RX){
// 			last_RX = psx.stick(PSS_RX);
// 			serial.write("RX: ");
// 			serial.write(itoa(last_RX, temp, 16));
// 			serial.write("\n\r");
// 		}
// 		if (psx.stick(PSS_RY) != last_RY){
// 			last_RY = psx.stick(PSS_RY);
// 			serial.write("RY: ");
// 			serial.write(itoa(last_RY, temp, 16));
// 			serial.write("\n\r");
// 		}
	}
}
