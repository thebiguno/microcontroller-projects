#include "RDA5807.h"

using namespace digitalcave;

RDA5807::RDA5807(I2C* i2c) :
	i2c(i2c)
{
	//Force soft reset
	setRegister(0x02, 0xC003);
	setRegister(0x03, 0x0000);
	setRegister(0x04, 0x0A00);
	setRegister(0x05, 0x8800);
	setRegister(0x06, 0x0000);
	setRegister(0x07, 0x4202);
	writeRegisters();

	delay_ms(50);

	//Turn off soft reset, ready for next write
	setRegister(0x02, 0xC001);
	//Turn off soft mute
	setRegister(0x05, 0x8000);
	writeRegister(0x02);
	writeRegister(0x05);

	delay_ms(50);

	//Read all the registers
	readRegisters();
}

uint16_t RDA5807::getRegister(uint8_t registerNumber){
	uint8_t i = 0;
	if (registerNumber >= 0x02 && registerNumber <= 0x07){
		i = (registerNumber - 2) * 2;
	}
	else if (registerNumber >= 0x0A && registerNumber <= 0x0F){
		i = (registerNumber - 4) * 2;
	}
	else {
		return 0xBEEF;
	}
	uint16_t result = registers[i] << 8;
	result |= registers[i + 1];
	return result;
}

void RDA5807::setRegister(uint8_t registerNumber, uint16_t value){
	uint8_t i = 0;
	if (registerNumber >= 0x02 && registerNumber <= 0x07){
		i = (registerNumber - 2) * 2;
	}
	else if (registerNumber >= 0x0A && registerNumber <= 0x0F){
		i = (registerNumber - 4) * 2;
	}
	else {
		return;
	}
	registers[i] = (uint8_t) (value >> 8);
	registers[i + 1] = (uint8_t) (value & 0xFF);
}

void RDA5807::readRegisters(){
	I2CMessage message(&(registers[12]), 12);
	i2c->read(RDA5807_ADDRESS, &message);			//Start reading at register 0x0A, for a length of 12 bytes
}

void RDA5807::writeRegisters(){
	I2CMessage message(&(registers[0]), 12);	//Start writing at register 0x02, for a length of 12 bytes.
	i2c->write(RDA5807_ADDRESS, &message);
}

void RDA5807::writeRegister(uint8_t registerNumber){
	if (registerNumber < 2 || registerNumber > 7){
		return;
	}
	uint8_t data[3];
	data[0] = registerNumber;
	data[1] = (getRegister(registerNumber) >> 8);
	data[2] = (getRegister(registerNumber) & 0xFF);
	I2CMessage message(data, sizeof(data));
	i2c->write(RDA5807_ADDRESS_RANDOM, &message);
}

void RDA5807::setStation(uint16_t station){
	if (station > 1080 || station < 870){
		station = 870;
	}
	setRegister(0x03, ((station - 870) << 6) | 0x10);	//Set the station as the top 10 bits in register 3, with 0x10 to indicate we want to start a tuning operation
	writeRegister(0x03);
}

void RDA5807::setVolume(uint8_t volume){
	if (volume > 0x0F){
		volume = 0x0F;
	}
	setRegister(0x05, (getRegister(0x05) & 0xFFF0) | volume);
	writeRegister(0x05);
}
