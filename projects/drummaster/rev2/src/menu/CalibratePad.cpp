#include "ChannelCalibration.h"

using namespace digitalcave;

ChannelCalibration::ChannelCalibration(){
}

Menu* ChannelCalibration::handleAction(){
	
	if (value == -1){
		value = readFromPotentiometer(channel);
		encoder.write(value);
	}
	
	snprintf(buf, sizeof(buf), "%d                   ", value >> 1);
	display.write_text(2, 0, buf, 20);
	
	encoderState = encoder.read();
	if (encoderState != value){
		writeToPotentiometer(channel, encoderState);
		//We always read back the value for verification
		value = readFromPotentiometer(channel);
		encoder.write(value);
		
		savePotentiometerToEeprom();
	}
	
	if (button.fallingEdge()){
		display.write_text(2, 0, "                    ", 20);
		return Menu::calibratePads;
	}
	
	return NULL;
}

void ChannelCalibration::loadPotentiometerFromEeprom(){
	uint16_t values[CHANNEL_COUNT];
	EEPROM.get(EEPROM_POTENTIOMETER, values);
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		writeToPotentiometer(i, values[i]);
	}
}

void ChannelCalibration::savePotentiometerToEeprom(){
	uint16_t values[CHANNEL_COUNT];
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		values[i] = readFromPotentiometer(i);
	}
	EEPROM.put(EEPROM_POTENTIOMETER, values);
}

uint8_t ChannelCalibration::getAddress(uint8_t channel){
	return POT_ADDRESS | (channel >> 1);
}

uint8_t ChannelCalibration::getMemoryAddress(uint8_t channel){
	return (channel & 0x01) << 4;
}

uint16_t ChannelCalibration::readFromPotentiometer(uint8_t channel){
	if (channel >= CHANNEL_COUNT) return 0xFFFF;
	uint8_t address = getAddress(channel);
	uint8_t memoryAddress = getMemoryAddress(channel);
	
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x0C);		//Read from given memory address
	Wire.endTransmission(false);
	Wire.requestFrom(address, (uint8_t) 2);
	uint16_t result = (Wire.read() << 8) | Wire.read();
	Wire.endTransmission();
	return result;
}

void ChannelCalibration::writeToPotentiometer(uint8_t channel, uint16_t value){
	if (channel >= CHANNEL_COUNT) return;
	uint8_t address = getAddress(channel);
	uint8_t memoryAddress = getMemoryAddress(channel);
	
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x00 | ((value >> 8) & 0x01));	//Write to the given memory address + bit 8 of message
	Wire.write(value & 0xFF);	//Write bits 0 - 7 of message
	Wire.endTransmission();	
}
