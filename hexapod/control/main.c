/*
 * Read the buttons on the PS2 Controller, and send them over the 
 * serial link (probably to an XBee).  This code is meant to constantly 
 * send the state of the buttons, even if the buttons are not pushed.
 * We let the receiving end decide what to do with this information.
 * Initially designed for a hexapod controller, but it should be usable 
 * for most robotics / gaming interface tasks.
 *
 * Protocol is as follows:
 * Buttons: [11CCXXXX]
 *   (two high bits, two checksum bits, four data bits)
 * Stick: [0000YYCC][XXXXXXXX] 
 *   (four low bits, two address bits, eight data bits)
 *   address bits are as follows:
 *     bit 0 = Right / Left (R = 0, L = 1)
 *     bit 1 = X / Y (X = 0, Y = 1)
 *   data bits are raw value of stick
 *
 * All checksums are 2 bit checksum on the rest of the packet, calculated using the 
 *  2 bit parity word algorithm.  Each 2-bit chunk of the packet is XOR'd together. 
 *  The receiver software will XOR all 8 2 bit words together; if the result is not 0x0
 *  then we know there was an error in transmission.
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

uint8_t stick_message;

int main (void){
	//Do setup here
	
	//Set up the timer to run at F_CPU / 256, in normal mode (we reset TCNT0 in the ISR)
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02);
	
	//Set compare value to be maximum
	OCR0A = 0xFF;

	serial_init_b(38400);

	psx_init(&PORTC, PORTC0, //Data (Brown)
		&PORTC, PORTC1, //Clock (Blue)
		&PORTC, PORTC2, //Command (Orange)
		&PORTC, PORTC3); //Attention (Yellow)

	uint16_t last_buttons = 0x00;

	//Main program loop
	while (1){
		psx_read_gamepad();
		uint16_t buttons = psx_buttons();
		
		if (buttons != last_buttons){ // || <enough time has passed>){
			last_buttons = buttons;
			
			if (buttons == 0x00){
				serial_write_c(0x00);
			}
			else {
				for (uint8_t x = 0; x < 16; x++){
					if (buttons & _BV(x)){
						uint8_t button_message = 0xC0;			//Start bits
						uint8_t checksum = 0x3;					//0x03 is the checksum of the start bits

						button_message |= (x & 0x0F);			//Button (0 - 15)

						checksum ^= (x >> 0x02) & 0x03;			//Add 2 MSB of button to checksum
						checksum ^= x & 0x03;					//Add 2 LSB of button to checksum
					
						button_message |= (checksum << 0x04);	//Clear last checksum
					
						serial_write_c(button_message);			//Send message
					}
				}
			}
		}
/*
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
*/
	}
}
