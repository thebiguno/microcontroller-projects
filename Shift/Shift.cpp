/*
 * An OO implementation of shift library.
 */

#include "Shift.h"
#include <stdlib.h>
#include <util/delay.h>

Shift::Shift(volatile uint8_t *dataPort, uint8_t dataPin, volatile uint8_t *clockPort, uint8_t clockPin) {
	this->dataPort = dataPort;
	this->dataDDR = dataPort - 0x1;
	this->dataIn = dataPort - 0x2;
	this->dataPin = dataPin;
	
	this->clockPort = clockPort;
	this->clockDDR = clockPort - 0x1;
	this->clockIn = clockPort - 0x2;
	this->clockPin = clockPin;
}

void Shift::send(uint8_t *data, uint16_t length){
	*this->dataDDR |= _BV(this->dataPin);
	*this->clockDDR |= _BV(this->clockPin);

	for (uint16_t i = 0; i < length; i++){
		for (uint8_t j = 0; j < 8; j++){
			if (data[i] & _BV(j))	*this->dataPort |= _BV(this->dataPin);
			else *this->dataPort &= ~_BV(this->dataPin);
			*this->clockPort |= _BV(this->clockPin);
			_delay_us(1);	//TODO do we need a delay here?  Probably not...
			*this->clockPort &= ~_BV(this->clockPin);
		}
	}
}

void Shift::receive(uint8_t *data, uint16_t length){
	*this->dataDDR &= ~_BV(this->dataPin);
	*this->clockDDR &= ~_BV(this->clockPin);
	
	for (uint16_t i = 0; i < length; i++){
		uint8_t value = 0;
		for (uint8_t j = 0; j < 8; j++){
			while (!*this->clockIn & _BV(this->clockPin));		//Busy-wait while clock is low
			//Once clock pulses high, read the bit
			if (*this->dataIn & _BV(this->dataPin)) value |= _BV(j);

			//TODO do we need to busy-wait until clock goes low again?
			while (*this->clockIn & _BV(this->clockPin));		//Busy-wait while clock is high
		}
		data[i] = value;
	}
}
