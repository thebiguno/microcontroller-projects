#include "comm.h"

static volatile double _velocity; 	//0 is stopped, 1 is full speed ahead
static volatile double _direction;	//0 is straight ahead, -1 is full left, 1 is full right
static volatile uint8_t _special;	//Special behaviours; set bits for each.  This message is only read once, then discarded.
static volatile uint8_t _new_message;	//0 is normal, non-zero is message waiting

void comm_init(){
	serial_init_b(38400);
}

uint8_t comm_available(){
	return _new_message;
}

void comm_read(double *velocity, double *direction, uint8_t *special){
	*velocity = _velocity;
	*direction = _direction;
	*special = _special;
	
	_special = 0x00;	//Once we read this once, we discard it.
	
	_new_message = 0x00;	//We have read this message
}

void _serial_init_rx(){
	//Enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts
	sei();
}

ISR(USART0_RX_vect){
	sei();	//We want PWM to continue uninterrupted while processing serial data
	
	uint8_t b = UDR0;

	if ((b & CONTROLLER_MESSAGE_TYPE_MASK) == CONTROLLER_MESSAGE_TYPE_ANALOG){	//Analog stick event
		if ((b & CONTROLLER_ANALOG_STICK) == CONTROLLER_ANALOG_STICK_LEFT){	//Left stick
			uint8_t value = b & CONTROLLER_ANALOG_VALUE_MASK;
			if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_X){	//X Axis
				//The PSX controller sends X=0 for all the way left; we want to translate this to -1 for left.
				_direction = 1.0/15 * value - 1;
			}
			else if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_Y){	//Y Axis
				//The PSX controller sends X=0 for all the way forward; we want to translate this to 1 for forward.
				_velocity = -1.0/15 * value + 1;
			}
		}
	}
	else if ((b & CONTROLLER_MESSAGE_TYPE_MASK) == CONTROLLER_MESSAGE_TYPE_BUTTON){	//Digital button event
		if ((b & CONTROLLER_BUTTON_PRESS_MASK) == CONTROLLER_BUTTON_PRESS){
			if ((b & CONTROLLER_BUTTON_VALUE_MASK) == CONTROLLER_BUTTON_VALUE_CROSS) _special |= SPECIAL_RESET;
		}
	}

	_new_message = 0x01;
}
