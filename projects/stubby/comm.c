#include "comm.h"

static volatile double _velocity; 	//0 is stopped, 1 is full speed ahead
static volatile double _direction;	//0 is straight ahead, -1 is full left, 1 is full right
static volatile uint8_t _special;	//Special behaviours; set bits for each

void comm_init(){
	serial_init_b(38400);
}

void comm_read(double *velocity, double *direction, uint8_t *special){
	*velocity = _velocity;
	*direction = _direction;
	*special = _special;
}

void _serial_init_rx(){
	//Enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

ISR(USART0_RX_vect){
	sei();	//We want PWM to continue uninterrupted while processing serial data
	static uint8_t button_press_mask = 0x00;
	
	uint8_t b = UDR0;
	
	//Button pressed - see what it is, and set the button press mask
	if ((b & CONTROLLER_MESSAGE_TYPE_MASK) == CONTROLLER_MESSAGE_TYPE_BUTTON){
		if (b == CONTROLLER_BUTTON_NONE){
			button_press_mask = 0x00;
		}
		else if ((b & CONTROLLER_BUTTON_PRESS_MASK) == CONTROLLER_BUTTON_PRESS){
			button_press_mask |= _BV(b & CONTROLLER_BUTTON_PRESS_MASK);
		}
		else if ((b & CONTROLLER_BUTTON_PRESS_MASK) == CONTROLLER_BUTTON_RELEASE){
			button_press_mask &= ~_BV(b & CONTROLLER_BUTTON_PRESS_MASK);
		}
	}
	
	//Check for special buttons
	if (button_press_mask & _BV(CONTROLLER_BUTTON_CROSS)){
		_special |= SPECIAL_RESET;
	}

	//Check for velocity buttons
	if (button_press_mask & _BV(CONTROLLER_BUTTON_PADUP)){
		_velocity = 1;
	}
	else if (button_press_mask & _BV(CONTROLLER_BUTTON_PADDOWN)){
		_velocity = -1;
	}
	else {
		//_velocity = 0;
	}
	
	//Check for directional buttons
	if (button_press_mask & _BV(CONTROLLER_BUTTON_RIGHT2)){
		_direction = 1;
	}
	else if (button_press_mask & _BV(CONTROLLER_BUTTON_LEFT2)){
		_direction = -1;
	}
	else {
		//_direction = 0;
	}
	
	//Reset everything if applicable
	if (button_press_mask == 0x00){
		_velocity = 0;
		_direction = 0;
		_special = 0;
	}
	
/*
	if ((b & CONTROLLER_MESSAGE_TYPE_MASK) == CONTROLLER_MESSAGE_TYPE_ANALOG){
		if ((b & CONTROLLER_ANALOG_STICK) == CONTROLLER_ANALOG_STICK_LEFT){	//Left stick
			if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_X){	//X Axis
				uint8_t value = b & CONTROLLER_ANALOG_VALUE_MASK;
				if (value > 0x08 && value < 0x17) {
					//Dead zone from 8 to 23 (i.e. 8 on either side of center).  This may be excessive; adjust as needed.
					_direction = 0x00;
				}
				else {
					//The PSX controller sends X=0 for all the way left; we want to translate this to -1 for left.
					_direction = 1.0/15 * value - 1;
				}
			}
			else if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_Y){	//Y Axis
				uint8_t value = b & CONTROLLER_ANALOG_VALUE_MASK;
				if (value > 0x0A && value < 0x15) {
					//Dead zone from 10 to 21 (i.e. 6 on either side of center).  This may be excessive; adjust as needed.
					_velocity = 0x00;
				}
				else {
					//The PSX controller sends X=0 for all the way forward; we want to translate this to 1 for forward.
					_velocity = -1.0/15 * value + 1;
				}
			}
		}
	}
*/
}
