/*
 * Read the buttons on the PS2 Controller, and send them over the 
 * serial link (probably to an XBee).  We send all button events
 * We let the receiving end decide what to do with this information.
 * Initially designed for a hexapod controller, but it should be usable 
 * for most robotics / gaming interface tasks.
 *
 * There is an idle poll which re-sends button state on a regular basis
 * (default of about 500ms) even if there was no change of state.  This
 * can ensure that the receiving client does not miss an event.  The idle
 * polling does not re-send analog data.
 *
 * Every message, whether outgoing or incoming, is contained within a single 
 * byte.  This allows us to avoid keeping track of message state on both sides,
 * and also allows us to ignore the need for checksums (checksumming of a single
 * byte is handled by the XBee; checksumming a multiple byte message would have
 * to be done by the application layer).
 *
 * Protocol is as follows:
 *
 * Sending: Controller status:
 *  
 *  No Buttons Pressed: [10000000]
 *    Static packet meaning 'no buttons pressed'.
 *  Button Press: [1111XXXX]
 *    1: Four high bits
 *    X: Four data bits, indicating button number 0 - 15
 *  Stick: [0AAXXXXX]
 *    0: One low bit
 *    A: Two address bits
 *      bit 0 = Left / Right (L = 0, R = 1)
 *      bit 1 = X / Y (X = 0, Y = 1)
 *    X: Five value bits
 *
 *
 *
 * Receiving: Message format: [CCXXXXXX]
 *     C: Command (see below)
 *     X: Command-specific message, left padded with zeros
 *
 *  Commands:
 *    00: Enable / Disable Buttons
 *      [000AAAAX]
 *      A: Button address.  Specify a button to modify (0x00 - 0x0F).
 *      X: New value.  1 = enabled, 0 = disabled
 *    01: Enable / Disable Analog Sticks
 *      [0100000X]
 *      X: New value.  1 = enabled, 0 = disabled
 *    10: Set poll frequency
 *      [10XXXXXX]
 *      X: New poll value, to be shifted 8 bits left and set in OCR1A.
 *         For instance, to set OCR1A = 0x0F00 (the default value), you would send the
 *         value 001111 (0x0F) as shifting this 8 bits gives 0x0F00.
 *         0x0F (expanded to 0x0F00) results in an idle poll time of about 500ms.
 *         Disable idle polling completely (only send change events) by 
 *         sending value 0x00.
 *    11: Reserved
 */


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/psx/psx.h"
#include "lib/serial/serial.h"

//Configuration state
static uint8_t analog_enabled = 0;
static uint16_t button_mask = 0xFFFF;

//Temporal controller state
static uint8_t last_lx = 0;
static uint8_t last_ly = 0;
static uint8_t last_rx = 0;
static uint8_t last_ry = 0;

static volatile uint16_t last_buttons = 0x00;

int main (void){
	//Do setup here
	
	//We use timer 0 to send a status byte at least every 
	TCCR1A = 0x0; //Normal mode (we reset TCNT1 when sending data)
	TCCR1B |= _BV(CS12) | _BV(CS10);	//F_CPU / 1024 prescaler
	OCR1A = 0x0F00;  //Set compare value; this will result in firing every ~500ms
	TIMSK1 = _BV(OCIE1A);  //Enable compare interrupt
	
	serial_init_b(38400);

	psx_init(&PORTC, PORTC0, //Data (Brown)
		&PORTC, PORTC1, //Clock (Blue)
		&PORTC, PORTC2, //Command (Orange)
		&PORTC, PORTC3); //Attention (Yellow)
		
	sei();
		
	//Main program loop
	while (1){
	
		//Check if there are any incoming command messages
		if (serial_available()){
			uint8_t message;
			serial_read_c((char*) &message);

			switch (message >> 6){
				case 0x00:
					if (message & 0x01){
						message = message >> 1;
						button_mask |= _BV(message);
					}
					else {
						message = message >> 1;
						button_mask &= ~_BV(message);
					}
					break;
				case 0x01:
					analog_enabled = message & 0x01;
					break;
				case 0x02:
					if ((message & 0x3F) == 0x00){
						OCR1A = 0x0000;
						TIMSK1 &= ~_BV(OCIE1A);  //Disable compare interrupt
					}
					else {
						OCR1A = ((message & 0x3F) << 8);
						TIMSK1 |= _BV(OCIE1A);  //Enable compare interrupt					
					}
					break;
			}
		}
	
		//Poll the gamepad
		psx_read_gamepad();
		
		//Check digital (button) state
		uint16_t buttons = psx_buttons() & button_mask;
		if (buttons != last_buttons){
			last_buttons = buttons;
			
			TCNT1 = 0x00;	//Reset idle timer
			
			if (buttons == 0x00){
				serial_write_c((char) 0x80);		//Static message meaning 'no buttons pressed'
			}
			else {
				for (uint8_t x = 0; x < 16; x++){
					if (buttons & _BV(x)){
						serial_write_c(x | 0xF0);	//Four high bits, then 4 bit button address
					}
				}
			}
		}

		//Check analog (stick) state
		if (analog_enabled){
			//We only send the 5 MSB of the sticks.
			uint8_t lx = psx_stick(PSS_LX) >> 3;
			uint8_t ly = psx_stick(PSS_LY) >> 3;
			uint8_t rx = psx_stick(PSS_RX) >> 3;
			uint8_t ry = psx_stick(PSS_RY) >> 3;

			if (lx != last_lx){
				last_lx = lx;
				serial_write_c(0x00 | lx);		//Address = 00
			}
			if (ly != last_ly){
				last_ly = ly;
				serial_write_c(0x20 | ly);		//Address = 01
			}
			if (rx != last_rx){
				last_rx = rx;
				serial_write_c(0x40 | rx);		//Address = 10
			}
			if (ry != last_ry){
				last_ry = ry;
				serial_write_c(0x60 | ry);		//Address = 11
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER1_COMPB_vect)
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
	//Force a re-send of button state
	last_buttons = 0xFF;
}

