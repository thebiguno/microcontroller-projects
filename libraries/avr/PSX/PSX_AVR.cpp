#include "PSX_AVR.h"

using namespace digitalcave;

/* Initialize the pins, and set the controller up to the correct mode.
 */
PSX_AVR::PSX_AVR(volatile uint8_t *data_port, uint8_t data_pin,				//MISO (Brown)
				volatile uint8_t *command_port, uint8_t command_pin,		//MOSI (Orange)
				volatile uint8_t *clock_port, uint8_t clock_pin,			//SCK (Blue)
				volatile uint8_t *attention_port, uint8_t attention_pin) {	//CS (Yellow)

	//Store the ports...
	this->data_port = data_port;
	this->command_port = command_port;
	this->clock_port = clock_port;
	this->attention_port = attention_port;
	
	//... and data direction registers...
	this->data_ddr = data_port - 0x1;
	this->command_ddr = command_port - 0x1;
	this->clock_ddr = clock_port - 0x1;
	this->attention_ddr = attention_port - 0x1;

	//... and data input register...
	this->data_in = data_port - 0x2;

	//... and pin numbers.
	this->data_pin = data_pin;
	this->command_pin = command_pin;
	this->clock_pin = clock_pin;
	this->attention_pin = attention_pin;
	
	
	//Initialize game pad - set SCK and MOSI high (SPI Mode 3)
	*this->command_port |= _BV(command_pin);
	*this->clock_port |= _BV(clock_pin);
	
	//Set clock, attention, and command pins to output mode
	*this->command_ddr |= _BV(command_pin);
	*this->clock_ddr |= _BV(clock_pin);
	*this->attention_ddr |= _BV(attention_pin);

	//Set data pin to input mode, and set pullup resistor
	*this->data_ddr &= ~(_BV(data_pin));
	*this->data_port |= _BV(data_pin);
	
	_delay_ms(100);
	
	init();
	
}

void PSX_AVR::sendCommand(uint8_t send_data[], uint8_t size){
	//Before you submit each command packet, you must set attention low; once
	// you are done each packet, return it high.  You have to toggle the line before
	// you submit another command.
	*attention_port &= ~(_BV(attention_pin));
	*command_port |= _BV(command_pin);

	//Clock should always be high; it is an active low line...
	*clock_port |= _BV(clock_pin);
		
	for (uint8_t i = 0; i < size; i++){
		
		//Do the actual serial transfer of each bit.  Handles clock.  The PSX controller is full 
		// duplex, so this will send a byte as well as receive one.  We send data on the clock
		// rising edge, and read data on the clock falling edge
 		uint8_t received_byte = 0;
		for (uint8_t j = 0; j < 8; j++) {
			//Drop the clock...
			*clock_port &= ~_BV(clock_pin);
		
			//...set the command (outgoing) pin...
			if (send_data[i] & (_BV(j))) {
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
				received_byte |= _BV(j);
			}
			
			//...and wait the other half of the clock cycle
			_delay_us(CTRL_CLK);
		}

		//Clock should already be high at this point, but just to be sure...
		*clock_port |= _BV(clock_pin);
		
		send_data[i] = received_byte;
	}
	
	*attention_port |= _BV(attention_pin);
}
