/*
 * An OO implementation of shift library.
 */

#include "Shift.h"
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

Shift::Shift(volatile uint8_t *dataPort, uint8_t dataPin, volatile uint8_t *clockPort, uint8_t clockPin) {
	this->dataPort = dataPort;
	this->dataDDR = dataPort - 0x1;
	this->dataPin = dataPin;
	
	this->clockPort = clockPort;
	this->clockDDR = clockPort - 0x1;
	this->clockPin = clockPin;
}

void send(uint8_t data*, uint16_t length){

}

uint16_t receive(uint8_t data*, uint16_t length){
	this->dataDDR |= 
}
