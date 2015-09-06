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

uint8_t last_LX = 0;
uint8_t last_LY = 0;
uint8_t last_RX = 0;
uint8_t last_RY = 0;

int main (void){
	//Do setup here
	
	Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, 
		&PORTF, PORTF0,
		&PORTF, PORTF1,
		&PORTF, PORTF4,
		&PORTF, PORTF5,
		&PORTF, PORTF6,
		&PORTF, PORTF7);
	CharDisplay display(&hd44780, 2, 16);

	PSX_AVR psx(&PORTD, PORTD2, //Data (Brown)
		&PORTD, PORTD3, //Command (Orange)
		&PORTD, PORTD4, //Clock (Blue)
		&PORTD, PORTD5); //Attention (Yellow)
	
 	char buf[17];
	
	//Main program loop
	while (1){
		_delay_ms(10);
		
		//usb_serial_write((const uint8_t*) "Foo\n", 5);

		psx.poll();
		
		if (psx.button(PSB_SELECT)) display.write_text(0, 0, "Select          ", 16);
		else if (psx.button(PSB_L1)) display.write_text(0, 0, "Left 1          ", 16);
		else if (psx.button(PSB_L2)) display.write_text(0, 0, "Left 2          ", 16);
		else if (psx.button(PSB_L3)) display.write_text(0, 0, "Left 3          ", 16);
		else if (psx.button(PSB_R1)) display.write_text(0, 0, "Right 1         ", 16);
		else if (psx.button(PSB_R2)) display.write_text(0, 0, "Right 2         ", 16);
		else if (psx.button(PSB_R3)) display.write_text(0, 0, "Right 3         ", 16);
		else if (psx.button(PSB_START)) display.write_text(0, 0, "Start           ", 16);
		else if (psx.button(PSB_PAD_UP)) display.write_text(0, 0, "Pad Up          ", 16);
		else if (psx.button(PSB_PAD_LEFT)) display.write_text(0, 0, "Pad Left        ", 16);
		else if (psx.button(PSB_PAD_DOWN)) display.write_text(0, 0, "Pad Down        ", 16);
		else if (psx.button(PSB_PAD_RIGHT)) display.write_text(0, 0, "Pad Right       ", 16);
		else if (psx.button(PSB_TRIANGLE)) display.write_text(0, 0, "Triangle        ", 16);
		else if (psx.button(PSB_CIRCLE)) display.write_text(0, 0, "Circle          ", 16);
		else if (psx.button(PSB_CROSS)) display.write_text(0, 0, "Cross           ", 16);
		else if (psx.button(PSB_SQUARE)) display.write_text(0, 0, "Square          ", 16);
		else display.write_text(0, 0, "                ", 16);

		snprintf(buf, sizeof(buf), "L:%02X,%02X R:%02X,%02X ", psx.stick(PSS_LX), psx.stick(PSS_LY), psx.stick(PSS_RX), psx.stick(PSS_RY));
		display.write_text(1, 0, buf, 16);
		
		display.refresh();
	}
}
