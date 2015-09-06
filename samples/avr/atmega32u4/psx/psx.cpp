/*
 * PS2 Controller test.  Connect the wires as indicated in the PSX constructor
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>

#include <PSX_AVR.h>
#include <Hd44780_Direct.h>
#include <CharDisplay.h>

using namespace digitalcave;

Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, 
	&PORTF, PORTF0,
	&PORTF, PORTF1,
	&PORTF, PORTF4,
	&PORTF, PORTF5,
	&PORTF, PORTF6,
	&PORTF, PORTF7);
CharDisplay display(&hd44780, 2, 16);
	
char temp[32];
uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here
	DDRB |= _BV(PORTB4) | _BV(PORTB5);

	//usb_init();
	//while (!usb_configured()) /* wait */ ;
	//_delay_ms(1000);
	
// 	Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, 
// 		&PORTF, PORTF0,
// 		&PORTF, PORTF1,
// 		&PORTF, PORTF4,
// 		&PORTF, PORTF5,
// 		&PORTF, PORTF6,
// 		&PORTF, PORTF7);
// 	CharDisplay display(&hd44780, 2, 16);

	PSX_AVR psx(&PORTD, PORTD2, //Data (Brown)
		&PORTD, PORTD3, //Command (Orange)
		&PORTD, PORTD4, //Clock (Blue)
		&PORTD, PORTD5); //Attention (Yellow)
	
	display.write_text(0, 0, "It Works!           ", 20);
	display.refresh();
	
// 	char buf[20];
	
//	psx.init();
	
	//Main program loop
	while (1){
		_delay_ms(10);
		
		//usb_serial_write((const uint8_t*) "Foo\n", 5);

		psx.poll();
		
		char buf[20];
		snprintf(buf, sizeof(buf), "%d;%02X,%02X;%02X,%02X                         ", psx.buttons(), psx.stick(PSS_LX), psx.stick(PSS_LY), psx.stick(PSS_RX), psx.stick(PSS_RY));
		display.write_text(1, 0, buf, 20);
		display.refresh();
		
		if (psx.stick(PSS_LX) != last_LX){
			last_LX = psx.stick(PSS_LX);
			PORTB ^= _BV(PORTB4);
		}
		
		if (psx.buttons()) PORTB &= ~_BV(PORTB5);
		else PORTB |= _BV(PORTB5);


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
