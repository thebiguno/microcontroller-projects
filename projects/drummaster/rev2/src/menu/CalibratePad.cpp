#include "CalibratePad.h"

using namespace digitalcave;

CalibratePad::CalibratePad(){
}

Menu* CalibratePad::handleAction(){
	
	if (value == -1){
		value = readValue(channel);
		encoder.write(value);
	}
	
	snprintf(buf, sizeof(buf), "%d                   ", value >> 1);
	display.write_text(2, 0, buf, 20);
	
	encoderState = encoder.read();
	if (encoderState != value){
		writeValue(channel, encoderState);
		//We always read back the value for verification
		value = readValue(channel);
		encoder.write(value);
	}
	
	if (button.fallingEdge()){
		display.write_text(2, 0, "                    ", 20);
		return Menu::calibratePads;
	}
	
	return NULL;
}

void CalibratePad::loadFromEeprom(){
	
}

void CalibratePad::saveToEeprom(){
	
}

uint8_t CalibratePad::getAddress(uint8_t channel){
	return POT_ADDRESS | (channel >> 1);
}

uint8_t CalibratePad::getMemoryAddress(uint8_t channel){
	return (channel & 0x01) << 4;
}

uint16_t CalibratePad::readValue(uint8_t channel){
	uint8_t address = getAddress(channel);
	uint8_t memoryAddress = getMemoryAddress(channel);
	
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x0C);		//Read from given memory address
	Wire.endTransmission(false);
	Wire.requestFrom(address, (uint8_t) 2);
	return (Wire.read() << 8) | Wire.read();
}

void CalibratePad::writeValue(uint8_t channel, uint16_t value){
	uint8_t address = getAddress(channel);
	uint8_t memoryAddress = getMemoryAddress(channel);
	
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x00 | ((value >> 8) & 0x01));	//Write to the given memory address + bit 8 of message
	Wire.write(value & 0xFF);	//Write bits 0 - 7 of message
	Wire.endTransmission();	
}
