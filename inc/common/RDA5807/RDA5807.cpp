#include "RDA5807.h"

using namespace digitalcave;

RDA5807::RDA5807(I2C* i2c) :
	i2c(i2c)
{
	//Force soft reset and initial state
	setRegister(0x02, 0xC003);
	setRegister(0x03, 0x0000);
	setRegister(0x04, 0x0A00);
	setRegister(0x05, 0x0800);
	setRegister(0x06, 0x0000);
	setRegister(0x07, 0x4202);

	//Turn off soft reset to bring chip up
	setRegister(0x02, 0xC005);
}

uint16_t RDA5807::getRegister(uint8_t registerNumber){
	if (registerNumber != 0x00 && (registerNumber < 0x02 || registerNumber > 0x07) && (registerNumber < 0x0A || registerNumber > 0x0F)){
		return 0xBEEF;
	}
	uint8_t data[2];
	data[0] = registerNumber;
	I2CMessage message(data, 1);
	i2c->write(RDA5807_ADDRESS, &message);
	message.setLength(2);
	i2c->read(RDA5807_ADDRESS, &message);

	return (((uint16_t) data[0]) << 8) | data[1];
}

void RDA5807::setRegister(uint8_t registerNumber, uint16_t value){
	if (registerNumber < 0x02 || registerNumber > 0x07){
		return;
	}
	uint8_t data[3];
	data[0] = registerNumber;
	data[1] = value >> 8;
	data[2] = value & 0xFF;
	I2CMessage message(data, sizeof(data));
	i2c->write(RDA5807_ADDRESS, &message);
	delay_ms(10);
}

uint16_t RDA5807::getStation(){
	//return ((getRegister(0x03) & 0xFFC0) >> 6) + MIN_STATION;
	return (getRegister(0x0A) & 0x03FF) + MIN_STATION;
}

void RDA5807::setStation(uint16_t station){
	if (station > MAX_STATION || station < MIN_STATION){
		station = MIN_STATION;
	}
	setRegister(0x03, ((station - MIN_STATION) << 6) | 0x10);	//Set the station as the top 10 bits in register 3, with 0x10 to indicate we want to start a tuning operation
}

void RDA5807::doScan(uint8_t direction){
	if (direction){
		setRegister(0x02, 0xC301);		//Start seeking up
	}
	else {
		setRegister(0x02, 0xC101);		//Start seeking down
	}
}

uint8_t RDA5807::getVolume(){
	return getRegister(0x05) & 0x000F;
}

void RDA5807::setVolume(uint8_t volume){
	if (volume > 0x0F){
		volume = 0x0F;
	}
	setRegister(0x05, (getRegister(0x05) & 0xFFF0) | volume);
}

uint8_t RDA5807::getMute(){
	return (getRegister(0x02) & 0x4000) ? 0 : 1;
}
void RDA5807::setMute(uint8_t mute_on){
	//The FM chip negates it, calling it "mute_disable", so we compare with mute_on == 0.
	setRegister(0x02, ((getRegister(0x02) & ~0x4000) | (mute_on ? 0x0000 : 0x4000)));
}

uint8_t RDA5807::getHiZ(){
	return (getRegister(0x02) & 0x8000) ? 0 : 1;
}
void RDA5807::setHiZ(uint8_t hi_z){
	//The FM chip negates it, calling it "1 = not hi z", so we compare with hi_z == 0.
	setRegister(0x02, ((getRegister(0x02) & ~0x8000) | (hi_z ? 0x0000 : 0x8000)));
}

uint8_t RDA5807::getEnabled(){
	return (getRegister(0x02) & 0x0001);
}

void RDA5807::setEnabled(uint8_t enabled){
	setRegister(0x02, ((getRegister(0x02) & ~0x0001) | (enabled ? 0x0001 : 0x0000)));
}

uint8_t RDA5807::getReset(){
	return (getRegister(0x02) & 0x0002) ? 1 : 0;
}

void RDA5807::setReset(uint8_t reset){
	setRegister(0x02, ((getRegister(0x02) & ~0x0002) | (reset ? 0x0002 : 0x0000)));
}

uint8_t RDA5807::getSignalStrength(){
	return (getRegister(0x0B) & 0xFE00) >> 9;
}
