#include "CalibrateChannel.h"

using namespace digitalcave;

static const char* labels[CHANNEL_COUNT] = {
	" Hi Hat             ",
	" Hi Hat Pedal       ",
	" Snare              ",
	" Bass               ",
	" Tom 1              ",
	" Crash              ",
	" Tom 2              ",
	" Tom 3              ",
	" Splash             ",
	" Ride               ",
	" X0                 ",
	" X1                 "
};

CalibrateChannel::CalibrateChannel() : Menu(255){
}

Menu* CalibrateChannel::handleAction(){
	display->write_text(0, 0, "Calibrate Channels   ", 20);
	display->write_text(1, 0, labels[channel], 20);
	
	if (value == -1){
		value = readFromPotentiometer(channel);
		setMenuPosition(value);
	}
	
	snprintf(buf, sizeof(buf), "%d                   ", value >> 1);
	display->write_text(2, 0, buf, 20);
	
	encoderState = getMenuPosition();
	if (getMenuPosition() != value){
		writeToPotentiometer(channel, getMenuPosition());
		//We always read back the value for verification
		value = readFromPotentiometer(channel);
		setMenuPosition(value);
		
	}
	
	if (button.longPressEvent()){
		display->clear();
		savePotentiometerToEeprom();

		return Menu::calibrateChannelSelect;
	}
	
	return NULL;
}

void CalibrateChannel::loadPotentiometerFromEeprom(){
	uint16_t values[CHANNEL_COUNT];
	EEPROM.get(EEPROM_POTENTIOMETER, values);
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		writeToPotentiometer(i, values[i]);
	}
}

void CalibrateChannel::savePotentiometerToEeprom(){
	uint16_t values[CHANNEL_COUNT];
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		values[i] = readFromPotentiometer(i);
	}
	EEPROM.put(EEPROM_POTENTIOMETER, values);
}

uint8_t CalibrateChannel::getAddress(uint8_t channel){
	return POT_ADDRESS | (channel >> 1);
}

uint8_t CalibrateChannel::getMemoryAddress(uint8_t channel){
	return (channel & 0x01) << 4;
}

uint16_t CalibrateChannel::readFromPotentiometer(uint8_t channel){
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

void CalibrateChannel::writeToPotentiometer(uint8_t channel, uint16_t value){
	if (channel >= CHANNEL_COUNT) return;
	uint8_t address = getAddress(channel);
	uint8_t memoryAddress = getMemoryAddress(channel);
	
	Wire.beginTransmission(address);
	Wire.write(memoryAddress | 0x00 | ((value >> 8) & 0x01));	//Write to the given memory address + bit 8 of message
	Wire.write(value & 0xFF);	//Write bits 0 - 7 of message
	Wire.endTransmission();	
}
