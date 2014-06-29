#include "comm.h"

//For X axis, 0 is neutral, negative numbers are left, positive are right.
//For Y axis, 0 is neutral, negative numbers are down (back), positive are up (forward).
static volatile Point left_stick(0,0,0);
static volatile Point right_stick(0,0,0);
static volatile uint16_t _pressed = 0x00;			//Bit mask of any buttons pressed.  Bits are cleared when read.
static volatile uint16_t _released = 0x00;			//Bit mask of any buttons released.  Bits are cleared when read.
static volatile uint16_t _held = 0x00;				//Bit mask of any buttons held.  Bits are cleared when released (in ISR).

void comm_init(){
	serial_init_b(38400);
}

void comm_configure_remote(){
	//Every few seconds we will send these controller init messages.
	static uint8_t controller_init = 0x00;		//Whenever this resets to 0, we re-send controler init messages
	controller_init++;
	if (controller_init >= 0xFF){
		serial_write_b(0x41);	//Enable analog sticks
		serial_write_b(0xFF);	//Set an analog repeat time of about 32ms.
		controller_init = 0x00;
	}	
}

uint16_t comm_read_pressed_buttons(){
	comm_configure_remote();
	
	if (_pressed == 0x00) return 0x00;
	
	uint16_t result = _pressed;
	_pressed = 0x00;
	return result;
}

uint16_t comm_read_released_buttons(){
	comm_configure_remote();
	
	if (_released == 0x00) return 0x00;
	
	uint16_t result = _released;
	_released = 0x00;
	return result;
}

uint16_t comm_read_held_buttons(){
	comm_configure_remote();
	
	return _held;
}

Point comm_read_left(){
	Point p(left_stick.x, left_stick.y, 0);
	return p;
}
Point comm_read_right(){
	Point p(right_stick.x, right_stick.y, 0);
	return p;
}

void _serial_init_rx(){
	//Enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts
	sei();
}

ISR(USART0_RX_vect){
	uint8_t b = UDR0;

	UCSR0B &= ~_BV(RXCIE0);	//Disable RX interrupts; we want PWM interrupts, but not RX interrupts.
	sei();	//We want PWM to continue uninterrupted while processing serial data

	if ((b & CONTROLLER_MESSAGE_TYPE_MASK) == CONTROLLER_MESSAGE_TYPE_ANALOG){	//Analog stick event
		//The PSX controller sends a 5 bit message: 0x00 for all the way left / forward, 0x1F for all the 
		// way right / back.  We want to offset this so that neutral is 0, so we subtract 15 from the raw 
		// value; for the Y axis, we negate the value to flip the orientation.
		uint8_t value = b & CONTROLLER_ANALOG_VALUE_MASK;

		if ((b & CONTROLLER_ANALOG_STICK) == CONTROLLER_ANALOG_STICK_LEFT){	//Left stick
			if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_X){	//X Axis
				left_stick.x = value - 15;
			}
			else if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_Y){	//Y Axis
				left_stick.y = 15 - value;
			}
		}
		else {	//Right stick
			if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_X){	//X Axis
				right_stick.x = value - 15;
			}
			else if ((b & CONTROLLER_ANALOG_AXIS) == CONTROLLER_ANALOG_AXIS_Y){	//Y Axis
				right_stick.y = 15 - value;
			}
		}
	}
	else {	//Digital button event
		if ((b & CONTROLLER_BUTTON_PRESS_MASK) == CONTROLLER_BUTTON_PRESS){	//Pressed
			_pressed |= _BV((b & CONTROLLER_BUTTON_VALUE_MASK));	//Mark as pressed; this is cleared when they are read.
			_held |= _BV((b & CONTROLLER_BUTTON_VALUE_MASK));	//Mark as held; this is cleared when released
		}
		else {	//Released
			_released |= _BV((b & CONTROLLER_BUTTON_VALUE_MASK));	//Mark as released; this is cleared when they are read.
			_held &= ~_BV((b & CONTROLLER_BUTTON_VALUE_MASK));	//Mark as no longer held
		}
	}
	
	//Re-enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
}
