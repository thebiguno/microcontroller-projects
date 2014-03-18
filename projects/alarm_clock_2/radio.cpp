#include "radio.h"

Radio(volatile uint8_t *resetPort, uint8_t resetPin){
	this->resetPort = resetPort;
	this->resetPin = resetPin;
}
	
void setChannel(uint16_t);
