#include "RDA5807.h"

using namespace digitalcave;

RDA5807::RDA5807(I2C* i2c) :
	i2c(i2c)
{
	//readRegisters();

	delay_ms(500);

	//Force soft reset and initial state
	// setRegister(0x02, 0xC003);
	// setRegister(0x03, 0x0000);
	// setRegister(0x04, 0x0A00);
	// setRegister(0x05, 0x8000);
	// setRegister(0x06, 0x0000);
	// setRegister(0x07, 0x4202);
	r.data[4] = 0xC0;
	r.data[5] = 0x03;
	r.data[6] = 0x00;
	r.data[7] = 0x00;
	r.data[8] = 0x0A;
	r.data[9] = 0x00;
	r.data[10] = 0x80;
	r.data[11] = 0x00;
	r.data[12] = 0x00;
	r.data[13] = 0x00;
	r.data[14] = 0x42;
	r.data[15] = 0x02;

	writeRegister(0x02);
	writeRegister(0x03);
	writeRegister(0x04);
	writeRegister(0x05);
	writeRegister(0x06);
	writeRegister(0x07);

	// writeRegisters();

	delay_ms(100);

	//Turn off soft reset to bring chip up
	setRegister(0x02, 0xC005);
	// r.fields.r2_soft_reset = 0;		//Turn off soft reset
	// r.fields.r2_new_method = 1;		//Turn on new method
	// writeRegister(0x02);

	// readRegisters();
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

void RDA5807::readRegisters(){
	uint8_t data[2];
	data[0] = 0x00;
	I2CMessage message(data, 1);
	i2c->write(RDA5807_ADDRESS, &message);

	I2CMessage messageRead(r.data, 32);
	i2c->read(RDA5807_ADDRESS, &messageRead);
}

void RDA5807::readRegister(uint8_t registerNumber){
	uint8_t data[3];
	data[0] = registerNumber;
	I2CMessage message(data, 1);
	i2c->write(RDA5807_ADDRESS, &message);

	message.setLength(3);
	i2c->read(RDA5807_ADDRESS, &message);

	r.data[registerNumber * 2] = data[1];
	r.data[registerNumber * 2 + 1] = data[2];
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

void RDA5807::writeRegister(uint8_t registerNumber){
	// r.fields.r4_rsvd1 = 0;
	// r.fields.r4_rsvd2 = 0;
	// r.fields.r4_rsvd3 = 0;
	// r.fields.r5_rsvd1 = 0;
	// r.fields.r5_rsvd2 = 0;
	// r.fields.r6_rsvd1 = 0;
	// r.fields.r7_rsvd1 = 0;
	// r.fields.r7_rsvd2 = 0;

	uint8_t data[3];
	data[0] = registerNumber;
	data[1] = r.data[registerNumber * 2];
	data[2] = r.data[registerNumber * 2 + 1];
	I2CMessage message(data, sizeof(data));
	i2c->write(RDA5807_ADDRESS, &message);

	delay_ms(10);
}

void RDA5807::writeRegisters(){
	uint8_t oldValue = r.data[3];
	r.data[3] = 0x02;		//We (ab)use the LSB of register 1 in the array to hold the destination register address.
	I2CMessage message(&(r.data[3]), 13);
	i2c->write(RDA5807_ADDRESS, &message);
	r.data[3] = oldValue;	//Replace old value in struct
}

uint8_t RDA5807::getChipId(){
	return r.fields.r0_chip_id;
}

uint16_t RDA5807::getStation(){
//	return r.fields.rA_read_chan_lsb;
	return ((((uint16_t) r.fields.rA_read_chan_msb) << 8) | r.fields.rA_read_chan_lsb) + MIN_STATION;
	//return ((getRegister(0x03) & 0xFFC0) >> 6) + MIN_STATION;
	//return (getRegister(0x0A) & 0x003F) + MIN_STATION;
}

void RDA5807::setStation(uint16_t station){
	if (station > MAX_STATION || station < MIN_STATION){
		station = MIN_STATION;
	}
	setRegister(0x03, ((station - MIN_STATION) << 6) | 0x10);	//Set the station as the top 10 bits in register 3, with 0x10 to indicate we want to start a tuning operation
	// r.fields.r3_channel_msb = ((station - MIN_STATION) >> 2) & 0xFF;
	// r.fields.r3_channel_lsb = (station - MIN_STATION) & 0x03;
	// r.fields.r3_tune = 1;
	// writeRegister(0x03);	//Set the station as the top 10 bits in register 3, with 0x10 to indicate we want to start a tuning operation
}

void RDA5807::doScan(){
	r.fields.r2_seek = 1;		//Seek
	r.fields.r2_seekup = 1;		//Seek up
	r.fields.r2_seek_mode = 0;	//Seek wrap
	writeRegister(0x02);
}

uint8_t RDA5807::getVolume(){
	return r.fields.r5_volume;
}

void RDA5807::setVolume(uint8_t volume){
	if (volume > 0x0F){
		volume = 0x0F;
	}
	readRegister(0x05);
	r.fields.r5_volume = volume;
	writeRegister(0x05);
}

uint8_t RDA5807::getMute(){
	return r.fields.r2_dmute;
}
void RDA5807::setMute(uint8_t mute_on){
	//The FM chip negates it, calling it "mute_disable", so we compare with mute_on == 0.
	r.fields.r2_dmute = (mute_on == 1);
	writeRegister(0x02);
}

rda5807_registers_t* RDA5807::getRaw(){
	return &r;
}
