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
 *     bit 0 = Left / Right (L = 0, R = 1)
 *     bit 1 = X / Y (X = 0, Y = 1)
 *   data bits are raw value of stick
 *
 * By default the controller does not send analog (stick) data; to turn on
 * analog controllers, send a non-zero byte.  To disable analog controllers, send
 * byte 0x00.  
 * TODO Do we want a checksummed protocol for receiving data?  Perhaps if there is more
 * than just this...
 *
 * All checksums are 2 bit checksum on the rest of the packet, calculated using the 
 *  2 bit parity word algorithm.  Each 2-bit chunk of the packet is XOR'd together. 
 *  The receiver software will XOR all 8 2 bit words together; if the result is not 0x0
 *  then we know there was an error in transmission.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/psx/psx.h"
#include "lib/serial/serial.h"

static uint8_t analog_enabled = 0;

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
		if (serial_available()){
			serial_read_c((char*) &analog_enabled);
		}
	
		psx_read_gamepad();
		uint16_t buttons = psx_buttons();
		uint8_t lx = psx_stick(PSS_LX);
		uint8_t ly = psx_stick(PSS_LY);
		uint8_t rx = psx_stick(PSS_RX);
		uint8_t ry = psx_stick(PSS_RY);

		if (buttons != last_buttons || (analog_enabled && (lx != last_lx || ly != last_ly || rx != last_rx || ry != last_ry))){
			last_buttons = buttons;
			
			TCNT1 = 0x00;	//Reset idle timer
			
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
					
						button_message |= (checksum << 0x04);	//Set current checksum
					
						serial_write_c(button_message);			//Send message
					}
				}
			}
		}

		if (analog_enabled){
			if (lx != last_lx){
				last_lx = lx;
				uint8_t stick_message = 0x00;		//LX
				uint8_t checksum = 0x00;			//0x00 is the checksum of the start bits
			
				checksum ^= (lx >> 0x06) & 0x03;	//Add 2 MSB of stick value to checksum
				checksum ^= (lx >> 0x04) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= (lx >> 0x02) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= lx & 0x03;				//Add 2 LSB of stick value to checksum

				stick_message |= checksum;			//Set current checksum
			
				serial_write_c(stick_message);		//Send message (two bytes);
				serial_write_c(lx);
			}
			if (ly != last_ly){
				last_ly = ly;
				uint8_t stick_message = 0x01;		//LY
				uint8_t checksum = 0x01;			//0x01 is the checksum of the start bits
			
				checksum ^= (ly >> 0x06) & 0x03;	//Add 2 MSB of stick value to checksum
				checksum ^= (ly >> 0x04) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= (ly >> 0x02) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= ly & 0x03;				//Add 2 LSB of stick value to checksum

				stick_message |= checksum;			//Set current checksum
			
				serial_write_c(stick_message);		//Send message (two bytes);
				serial_write_c(ly);
			}
			if (rx != last_rx){
				last_rx = rx;
				uint8_t stick_message = 0x02;		//RX
				uint8_t checksum = 0x02;			//0x02 is the checksum of the start bits
			
				checksum ^= (rx >> 0x06) & 0x03;	//Add 2 MSB of stick value to checksum
				checksum ^= (rx >> 0x04) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= (rx >> 0x02) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= rx & 0x03;				//Add 2 LSB of stick value to checksum

				stick_message |= checksum;			//Set current checksum
			
				serial_write_c(stick_message);		//Send message (two bytes);
				serial_write_c(rx);
			}
			if (ry != last_ry){
				last_ry = ry;
				uint8_t stick_message = 0x03;		//RY
				uint8_t checksum = 0x03;			//0x03 is the checksum of the start bits
			
				checksum ^= (ry >> 0x06) & 0x03;	//Add 2 MSB of stick value to checksum
				checksum ^= (ry >> 0x04) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= (ry >> 0x02) & 0x03;	//Add next 2 bits of stick value to checksum
				checksum ^= ry & 0x03;				//Add 2 LSB of stick value to checksum

				stick_message |= checksum;			//Set current checksum
			
				serial_write_c(stick_message);		//Send message (two bytes);
				serial_write_c(ry);
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER1_COMPB_vect)
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
	last_buttons = 0xFF;	//Force a re-send of state
	last_lx = 0xFF;
//	last_ly = 0xFF;
//	last_rx = 0xFF;
//	last_ry = 0xFF;
}

