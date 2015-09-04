/*
 * Read the buttons on the PS2 Controller, and send them over the 
 * serial link (probably to an XBee).  We send all button events.
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

	//TODO Serial setup
	//TODO PSX Setup

	sei();
	uint8_t announceControlIdTimer = 0x00;
		
	//Main program loop
	while (1){
	}
}
