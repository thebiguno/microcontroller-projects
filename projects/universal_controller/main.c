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
 * See lib/protocol/protocol.txt for protocol details and lib/universal_controller/protocol.txt 
 * for Universal Controller-specific commands.
 *
 * We announce the controller id ('U', for Universal Controller) every once in a while.
 *
 * By default, all 16 buttons and both joysticks are enabled.  To change this,
 * use the enable / disable button / joystick commands.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/serial/serial.h"
#include "lib/universal_controller/client.h"
#include "lib/protocol/protocol.h"
#include "lib/psx/psx.h"

//Configuration state
static uint8_t analog_enabled = 0x01;
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

void _serial_init_rx(){
	//Enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

void protocol_write(uint8_t byte){
	serial_write_b(byte);
}

void protocol_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	switch (cmd){
		case MESSAGE_UC_BUTTON_ENABLE:
			button_mask |= _BV(message[0]);
			break;
		case MESSAGE_UC_BUTTON_DISABLE:
			button_mask &= ~_BV(message[0]);
			break;
		case MESSAGE_UC_JOYSTICK_ENABLE:
			analog_enabled = 0x01;
			break;
		case MESSAGE_UC_JOYSTICK_DISABLE:
			analog_enabled = 0x00;
			break;
		case MESSAGE_UC_SET_POLL_FREQUENCY:
			if (message[0] == 0x00){
				OCR1A = 0x0000;
				TIMSK1 &= ~_BV(OCIE1A);  //Disable compare interrupt
			}
			else {
				OCR1A = (message[0] << 6);
				TIMSK1 |= _BV(OCIE1A);  //Enable compare interrupt
			}
			break;
		case MESSAGE_UC_SET_ANALOG_FREQUENCY:
			if (message[0] == 0x00){
				OCR0A = 0x00;
				TIMSK0 &= ~_BV(OCIE0A);  //Disable compare interrupt
			}
			else {
				OCR0A = message[0];
				TIMSK0 |= _BV(OCIE1A);  //Enable compare interrupt
			}
			break;
	}
}

int main (void){
	//Do setup here
	
	//We use timer 0 to limit the sending analog status to a maximum of every X millis (default about 36ms, adjustable with MESSAGE_UC_SET_ANALOG_FREQUENCY command)
	TCCR0A = 0x0; //Normal mode (we reset TCNT0 when sending data)
	TCCR0B |= _BV(CS02) | _BV(CS00);	//F_CPU / 1024 prescaler
	OCR0A = 0xFF;		//Set analog stick compare value; this will result in showing analog values at most once every ~8ms
	TIMSK0 = _BV(OCIE0A);  //Enable compare interrupt on A
	
	
	//We use timer 1 to send a status byte every X millis (default about 500ms, adjustable with MESSAGE_UC_SET_POLL_FREQUENCY command)
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
	uint8_t announceControlIdTimer = 0x00;
		
	//Main program loop
	while (1){
		//If it is time to send the control ID again, do so and reset the counter.  By sending every 256 loops, 
		// it works out to be about once ever 2 - 3 seconds.
		if (announceControlIdTimer == 0x00){
			uint8_t value = 'U';
			protocol_send_message(MESSAGE_ANNOUNCE_CONTROL_ID, &value, 1);
		}
		announceControlIdTimer++;
	
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
						protocol_send_message(MESSAGE_UC_BUTTON_PUSH, &x, 1);
					}
					else {
						protocol_send_message(MESSAGE_UC_BUTTON_RELEASE, &x, 1);
					}
				}
				//If this was trggered by a poll event, only show buttons currently pressed
				else if (digital_poll_event){
					if (buttons & _BV(x)){
						protocol_send_message(MESSAGE_UC_BUTTON_PUSH, &x, 1);
					}
				}
			}

			//Show 'no buttons pressed' regardless of whether it was newly generated (release event) or if it was generated by poll timeout.
			if (buttons == 0x00){
				protocol_send_message(MESSAGE_UC_BUTTON_NONE, 0x00, 0);
			}

			last_buttons = buttons;
			
			digital_poll_event = 0x00;
		}

		//Check analog (stick) state and send 
		if (analog_enabled && (analog_release_event || (OCR0A == 0))){
			uint8_t lx = psx_stick(PSS_LX);
			uint8_t ly = psx_stick(PSS_LY);
			uint8_t rx = psx_stick(PSS_RX);
			uint8_t ry = psx_stick(PSS_RY);

			//By masking off the two LSB, we prevent re-sending analog values on minimal change.  From experience, the 2 LSB are mostly just noise anyway.
			#define MASK 0xFC
			if (analog_poll_event || (lx & MASK) != (last_lx & MASK) || (ly & MASK) != (last_ly & MASK) || (rx & MASK) != (last_rx & MASK) || (ry & MASK) != (last_ry & MASK)){
				last_lx = lx;
				last_ly = ly;
				last_rx = rx;
				last_ry = ry;
				uint8_t values[4];
				values[0] = lx;
				values[1] = ly;
				values[2] = rx;
				values[3] = ry;
				protocol_send_message(MESSAGE_UC_JOYSTICK_MOVE, values, 4);
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

ISR(USART_RX_vect){
	uint8_t b = UDR0;
	protocol_receive_byte(b);
}


