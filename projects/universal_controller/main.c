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
 *  [TAAVVVVV]
 *  [T00SBBBB]
 *  Bit 8 (MSB): 0 == analog stick message, 1 == digital button message
 *
 *  Analog stick message:
 *  Bit 7      : 0 == Left stick, 1 == Right Stick
 *  Bit 6      : 0 == X axis, 1 == Y axis
 *  Bit 5::1   : Analog stick value: For the Y axis, 0x00 is all the way forward and 
 *               0x1F is all the way back; for the X axis, 0x00 is all the way left 
 *               and 0x1F is all the way right.
 *
 *  Digital button message:
 *  Bit 7      : 0 == Normal button press / release event, 1 == No Buttons Pressed event.
 *               When bit 7 is 1, the remainder of the message is ignored.  By convention
 *               we fill buts 6::1 with zeros in this state, meaning that the 'No button
 *               pressed' message will always be 0xC0.
 *  Bit 6      : Unused, set to 0
 *  Bit 5      : 0 = Released, 1 = Pressed
 *  Bit 4::1   : Button number.  Use lib/universal_controller/client.h for defines.
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
 *    10: Set poll frequency (non-changes will be re-sent after this time)
 *      [10XXXXXX]
 *      X: New poll value, to be shifted 8 bits left and set in OCR1A.
 *         For instance, to set OCR1A = 0x0F00 (the default value), you would send the
 *         value 001111 (0x0F) as shifting this 8 bits gives 0x0F00.
 *         0x0F (expanded to 0x0F00) results in an idle poll time of about 500ms.
 *         0x3F (expanded to 0x3F00) results in an idle poll time of about 2s.
 *         0x01 (expanded to 0x0100) results in an idle poll time of about 10ms.
 *         0x00 results in completely disabling idle polling (only send change events).
 *    11: Set maximum analog repeat frequency (you can't send analog stick changes faster than this).
 *      [11XXXXXX]
 *      X: New poll value, to be shifted 2 bits left and set in OCR0A.  Analog values cannot be sent until 
 *         this compare ISR fires.  Set this high enough to not be sending un-needed data constantly, 
 *         but low enough that you can get good response times from the controller.
 *         0x3F (expanded to 0xFC) is the maximum, and results in an analog repeat time of about 32ms.
 *         0x10 (expanded to 0x40) is the default, and results in an analog repeat time of about 8ms.
 *         0x00 results in completely disabling the timer and sends all analog events, no matter how fast they come.
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

static uint16_t last_buttons = 0x00;

static volatile uint8_t digital_poll_event = 0x00;	//Set this to non-zero to force sending button pressed status
static volatile uint8_t analog_poll_event = 0x00;	//Set this to non-zero to force sending analog stick state
static volatile uint8_t analog_release_event = 0x00;	//Set this to non-zero to send analog state

int main (void){
	//Do setup here
	
	//We use timer 0 to send analog status every X millis (default about 36ms, adjustable with 11xxxxxx command)
	TCCR0A = 0x0; //Normal mode (we reset TCNT0 when sending data)
	TCCR0B |= _BV(CS02) | _BV(CS00);	//F_CPU / 1024 prescaler
	OCR0A = 0x40;		//Set analog stick compare value; this will result in showing analog values at most once every ~8ms
	TIMSK0 = _BV(OCIE0A);  //Enable compare interrupt on A
	
	
	//We use timer 1 to send a status byte every X millis (default about 500ms, adjustable with 10xxxxxx command)
	TCCR1A = 0x0; //Normal mode (we reset TCNT1 when sending data)
	TCCR1B |= _BV(CS12) | _BV(CS10);	//F_CPU / 1024 prescaler
	OCR1A = 0x0F00;		//Set poll compare value; this will result in forcing status to be fired every ~500ms
	TIMSK1 = _BV(OCIE1A);  //Enable compare interrupt on A
	
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
				case 0x03:
					if ((message & 0x3F) == 0x00){
						OCR0A = 0x00;
						TIMSK0 &= ~_BV(OCIE0A);  //Disable compare interrupt
					}
					else {
						OCR0A = ((message & 0x3F) << 2);
						TIMSK0 |= _BV(OCIE1A);  //Enable compare interrupt
					}
					break;
			}
		}
		
		//Poll the gamepad
		psx_read_gamepad();
		
		//Check digital (button) state
		uint16_t buttons = psx_buttons() & button_mask;
		if (buttons != last_buttons || digital_poll_event){
			uint16_t button_changes = buttons ^ last_buttons;
			
			TCNT1 = 0x00;	//Reset idle timer
			
			for (uint8_t x = 0; x < 16; x++){
				//If this was triggered by a real button event, show the current state (newly pressed or newly released)
				if (button_changes & _BV(x)){
					if (buttons & _BV(x)){
						serial_write_c(x | 0x90);	//b1001, then 4 bit button address
					}
					else {
						serial_write_c(x | 0x80);	//b1000, then 4 bit button address
					}
				}
				//If this was trggered by a poll event, only show buttons currently pressed
				else if (digital_poll_event){
					if (buttons & _BV(x)){
						serial_write_c(x | 0x90);	//b1001, then 4 bit button address
					}
				}
			}

			//Show 'no buttons pressed' regardless of whether it was newly generated (release event) or if it was generated by poll timeout.
			if (buttons == 0x00){
				serial_write_c((char) 0xC0);		//Static message meaning 'no buttons pressed'
			}

			last_buttons = buttons;
			
			digital_poll_event = 0x00;
		}

		//Check analog (stick) state and send 
		if (analog_enabled && (analog_release_event || (OCR0A == 0))){
			//We only send the 5 MSB of the sticks.
			uint8_t lx = psx_stick(PSS_LX) >> 3;
			uint8_t ly = psx_stick(PSS_LY) >> 3;
			uint8_t rx = psx_stick(PSS_RX) >> 3;
			uint8_t ry = psx_stick(PSS_RY) >> 3;

			if (lx != last_lx || analog_poll_event){
				last_lx = lx;
				serial_write_c(0x00 | lx);		//Address = 00
			}
			if (ly != last_ly || analog_poll_event){
				last_ly = ly;
				serial_write_c(0x20 | ly);		//Address = 01
			}
			if (rx != last_rx || analog_poll_event){
				last_rx = rx;
				serial_write_c(0x40 | rx);		//Address = 10
			}
			if (ry != last_ry || analog_poll_event){
				last_ry = ry;
				serial_write_c(0x60 | ry);		//Address = 11
			}
			
			analog_release_event = 0x00;
			analog_poll_event = 0x00;
		}
	}
}

EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
ISR(TIMER0_COMPA_vect){
	//Send analog data
	analog_release_event = 0x01;
}

EMPTY_INTERRUPT(TIMER1_COMPB_vect)
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
	//Force a re-send of button state
	digital_poll_event = 0x01;
	analog_poll_event = 0x01;
}

