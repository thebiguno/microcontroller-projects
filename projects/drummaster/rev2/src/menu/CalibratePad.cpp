#include "CalibratePad.h"

using namespace digitalcave;

extern Menu calibratePads;

CalibratePad::CalibratePad(){
}

Menu* CalibratePad::handleAction(){
	
	uint8_t address = POT_ADDRESS | (selectedPad >> 1);
	uint8_t memoryAddress = (selectedPad & 0x01) << 4;
	
	if (value == -1){
		value = readValue(address, memoryAddress);
		encoder.write(value);
	}
	
	snprintf(buf, sizeof(buf), "%d                   ", value >> 1);
	display.write_text(2, 0, buf, 20);
	
	encoderState = encoder.read();
	if (encoderState != value){
		writeValue(address, memoryAddress, encoderState);
		//We always read back the value for verification
		value = readValue(address, memoryAddress);
		encoder.write(value);
	}
	
	if (button.fallingEdge()){
		display.write_text(2, 0, "                    ", 20);
		return &calibratePads;
	}
	
	return NULL;
}

uint16_t CalibratePad::readValue(uint8_t address, uint8_t memoryAddress){
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x0C);		//Read from given memory address
	Wire.endTransmission(false);
	Wire.requestFrom(address, (uint8_t) 2);
	return (Wire.read() << 8) | Wire.read();
}

void CalibratePad::writeValue(uint8_t address, uint8_t memoryAddress, uint16_t value){
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x00 | ((value >> 8) & 0x01));	//Write to the given memory address + bit 8 of message
	Wire.write(value & 0xFF);	//Write bits 0 - 7 of message
	Wire.endTransmission();	
}
