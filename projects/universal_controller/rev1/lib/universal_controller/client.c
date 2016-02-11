#include <avr/interrupt.h>

#include "../serial/serial.h"

#include "client.h"

//For X axis, 0 is neutral, negative numbers are left, positive are right.
//For Y axis, 0 is neutral, negative numbers are down (back), positive are up (forward).
static volatile uc_stick_t left_stick;
static volatile uc_stick_t right_stick;
static volatile uint16_t _pressed = 0x00;			//Bit mask of any buttons pressed.  Bits are cleared when read.
static volatile uint16_t _released = 0x00;			//Bit mask of any buttons released.  Bits are cleared when read.
static volatile uint16_t _held = 0x00;				//Bit mask of any buttons held.  Bits are cleared when released (in ISR).

void uc_init(){
	serial_init_b(38400);
}

void uc_configure_remote(){
	//Every few seconds we will send these controller init messages.
	static uint8_t controller_init = 0x00;		//Whenever this resets to 0, we re-send controler init messages
	controller_init++;
	if (controller_init >= 0xFF){
		//NOTE: You should change these values depending on your application.  These defaults make sense for most projects, though.
		serial_write_b(0x41);	//Enable analog sticks
		serial_write_b(0xFF);	//Set an analog repeat time of about 32ms.
		controller_init = 0x00;
	}	
}

uint16_t uc_read_pressed_buttons(){
	uc_configure_remote();
	
	if (_pressed == 0x00) return 0x00;
	
	uint16_t result = _pressed;
	_pressed = 0x00;
	return result;
}

uint16_t uc_read_released_buttons(){
	uc_configure_remote();
	
	if (_released == 0x00) return 0x00;
	
	uint16_t result = _released;
	_released = 0x00;
	return result;
}

uint16_t uc_read_held_buttons(){
	uc_configure_remote();
	
	return _held;
}

uc_stick_t uc_read_left(){
	uc_stick_t result;
	result.x = left_stick.x;
	result.y = left_stick.y;
	return result;
}
uc_stick_t uc_read_right(){
	uc_stick_t result;
	result.x = right_stick.x;
	result.y = right_stick.y;
	return result;
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
		uint16_t value = _BV((b & CONTROLLER_BUTTON_VALUE_MASK));
		if ((b & CONTROLLER_BUTTON_PRESS_MASK) == CONTROLLER_BUTTON_PRESS){	//Pressed
			_pressed |= value;	//Mark as pressed; this is cleared when they are read.
			_held |= value;	//Mark as held; this is cleared when released
		}
		else {	//Released
			_released |= value;	//Mark as released; this is cleared when they are read.
			_held &= ~value;	//Mark as no longer held
		}
	}
	
	//Re-enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
}
