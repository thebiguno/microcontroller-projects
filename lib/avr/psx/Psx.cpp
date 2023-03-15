#include "Psx.h"

#include <avr/io.h>
#include <util/delay.h>

//Half a clock cycle; 10µs seems to be about right
#define CTRL_CLK        10

using namespace digitalcave;

/* Initialize the pins, and set the controller up to the correct mode.
 * (TODO: separate the config options into optional functions, and call based on flags,
 * to allow calling programs to determine what options to use)
 */
Psx::Psx(volatile uint8_t *data_port, uint8_t data_pin,
				volatile uint8_t *clock_port, uint8_t clock_pin,
				volatile uint8_t *command_port, uint8_t command_pin,
				volatile uint8_t *attention_port, uint8_t attention_pin) {

	//Store the ports...
	this->data_port = data_port;
	this->clock_port = clock_port;
	this->command_port = command_port;
	this->attention_port = attention_port;
	
	//... and data direction registers...
	this->data_ddr = data_port - 0x1;
	this->clock_ddr = clock_port - 0x1;
	this->command_ddr = command_port - 0x1;
	this->attention_ddr = attention_port - 0x1;

	//... and data input register...
	this->data_in = data_port - 0x2;

	//... and pin numbers.
	this->clock_pin = clock_pin;
	this->command_pin = command_pin;
	this->attention_pin = attention_pin;
	this->data_pin = data_pin;
	
	
	//Initialize game pad
	*this->clock_port |= _BV(clock_pin);
	*this->command_port |= _BV(command_pin);
	
	//Set clock, attention, and command pins to output mode
	*this->clock_ddr |= _BV(clock_pin);
	*this->attention_ddr |= _BV(attention_pin);
	*this->command_ddr |= _BV(command_pin);

	//Set data pin to input mode, and set pullup resistor
	*this->data_ddr &= ~(_BV(data_pin));
	*this->data_port |= _BV(data_pin);
	
	init();
}

Psx::~Psx() {
	
}

void Psx::sendCommand(uint8_t send_data[], uint8_t size){
	//Before you submit each command packet, you must set attention low; once
	// you are done each packet, return it high.  You have to toggle the line before
	// you submit another command.
	*attention_port &= ~(_BV(attention_pin));
	*command_port |= _BV(command_pin);

	//Clock should always be high; it is an active low line...
	*clock_port |= _BV(clock_pin);
		
	for (uint8_t i = 0; i < size; i++){
		send_data[i] = shift(send_data[i]);
	}
	
	*attention_port |= _BV(attention_pin);
}

/* The actual serial transfer.  Handles clock.  The PSX controller is full 
 * duplex, so this will send a byte as well as receive one.
 */
uint8_t Psx::shift(uint8_t transmit_byte) {
	uint8_t received_byte = 0;
	for (uint8_t i = 0; i < 8; i++) {
		//Drop the clock...
		*clock_port &= ~_BV(clock_pin);
	
		//...set the command (outgoing) pin...
		if (transmit_byte & (_BV(i))) {
			*command_port |= _BV(command_pin);
		}
		else {
			*command_port &= ~_BV(command_pin);
		}
		
		//...wait half the clock cycle...
		_delay_us(CTRL_CLK);
		
		//...raise the clock to HIGH...
		*clock_port |= _BV(clock_pin);
		
		//...at which point you read the data...
		if(*data_in & _BV(data_pin)) {
			received_byte |= _BV(i);
		}
		
		//...and wait the other half of the clock cycle
		_delay_us(CTRL_CLK);
	}

	//Clock should already be high at this point, but just to be sure...
	*clock_port |= _BV(clock_pin);
	
	return received_byte;
}
