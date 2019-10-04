#include "SI4703.h"

using namespace digitalcave;

Si4703_Breakout::Si4703_Breakout(volatile uint8_t *resetPort, uint8_t resetPin){
	this->resetPort = resetPort;
	this->resetPin = resetPin;

	//Set up /RST pin in output mode
	*(resetPort - 0x01) |= _BV(resetPin);

	//Configure the Si4703 for 2-wire communcation
	DDRC |= _BV(PORTC4);	//SDIO is connected to A4 for I2C
	PORTC &= ~_BV(PORTC4); //A low SDIO indicates a 2-wire interface
	*resetPort &= ~_BV(resetPin); //Put Si4703 into reset
	_delay_ms(1); //Some delays while we allow pins to settle
	*resetPort |= _BV(resetPin); //Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
	_delay_ms(1); //Allow Si4703 to come out of reset

	//Configure the I2C connection
	twi_init(); //Now that the unit is reset and I2C inteface mode, we need to begin I2C

	//Configure the si4703 to use the crystal
	readRegisters(); //Read the current register set
	this->si4703_registers[0x07] = 0x8100; //Enable the oscillator, from AN230 page 9, rev 0.61 (works)
	updateRegisters(); //Update
	_delay_ms(500); //Wait for clock to settle - from AN230 page 9

	//Power up the si4703 with our default settings
	//(No Mute, RDS Enabled, 200kHz channel spacing, lowest volume
	readRegisters(); //Read the current register set
	this->si4703_registers[POWERCFG] = 0x4001; //Enable the IC
	//this->si4703_registers[SYSCONFIG1] |= (1<<RDS); //Enable RDS

	this->si4703_registers[SYSCONFIG2] &= ~(1<<SPACE1 | 1<<SPACE0) ; //Force 200kHz channel spacing for USA

	this->si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
	this->si4703_registers[SYSCONFIG2] |= 0x0001; //Set volume to lowest
	updateRegisters(); //Update

	//Give the si4703 enough time to power up.
	_delay_ms(110); //Max powerup time, from datasheet page 13

	readRegisters();
}
void Si4703_Breakout::powerOn()
{

}


//Reads the current channel from READCHAN
//Returns a number like 973 for 97.3MHz
uint16_t Si4703_Breakout::getChannel() {
	//Freq (MHZ) = channel / 10 = 0.2 * 10_bit_register_value + 87.5 MHz, therefore
	//Freq (KHz * 100) = channel = 2 * 10_bit_register_value * 10 + 8750
	// channel = 20 * 10_bit_register_value + 8750

  readRegisters();
  uint16_t regValue = si4703_registers[CHANNEL] & 0x03FF; //Mask out everything but the lower 10 bits
  return 2 * regValue + 875;
}

void Si4703_Breakout::setChannel(uint16_t channel) {
	uint16_t regValue = (channel - 875) / 2;

	//These steps come from AN230 page 20 rev 0.5
	readRegisters();
	si4703_registers[CHANNEL] &= 0xFE00; //Clear out the channel bits
	si4703_registers[CHANNEL] |= regValue; //Mask in the new channel
	si4703_registers[CHANNEL] |= (1<<TUNE); //Set the TUNE bit to start
	updateRegisters();

	//_delay_ms(60); //Wait 60ms - you can use or skip this delay

	//Poll to see if STC is set
	while(1) {
		readRegisters();
		if( (si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
	}

	readRegisters();
	si4703_registers[CHANNEL] &= ~(1<<TUNE); //Clear the tune after a tune has completed
	updateRegisters();

	//Wait for the si4703 to clear the STC as well
	while(1) {
		readRegisters();
		if( (si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
	}
}

uint16_t Si4703_Breakout::seekUp()
{
	return seek(SEEK_UP);
}

uint16_t Si4703_Breakout::seekDown()
{
	return seek(SEEK_DOWN);
}

void Si4703_Breakout::setVolume(uint16_t volume)
{
  readRegisters(); //Read the current register set
  if(volume < 0) volume = 0;
  if (volume > 15) volume = 15;
  si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
  si4703_registers[SYSCONFIG2] |= volume; //Set new volume
  updateRegisters(); //Update
}
/*
void Si4703_Breakout::readRDS(char* buffer, long timeout){
	long endTime = millis() + timeout;
  boolean completed[] = {false, false, false, false};
  int completedCount = 0;
  while(completedCount < 4 && millis() < endTime) {
	readRegisters();
	if(si4703_registers[STATUSRSSI] & (1<<RDSR)){
		// ls 2 bits of B determine the 4 letter pairs
		// once we have a full set return
		// if you get nothing after 20 readings return with empty string
	  uint16_t b = si4703_registers[RDSB];
	  int index = b & 0x03;
	  if (! completed[index] && b < 500)
	  {
		completed[index] = true;
		completedCount ++;
	  	char Dh = (si4703_registers[RDSD] & 0xFF00) >> 8;
      	char Dl = (si4703_registers[RDSD] & 0x00FF);
		buffer[index * 2] = Dh;
		buffer[index * 2 +1] = Dl;
		// Serial.print(si4703_registers[RDSD]); Serial.print(" ");
		// Serial.print(index);Serial.print(" ");
		// Serial.write(Dh);
		// Serial.write(Dl);
		// Serial.println();
      }
      _delay_ms(40); //Wait for the RDS bit to clear
	}
	else {
	  _delay_ms(30); //From AN230, using the polling method 40ms should be sufficient amount of time between checks
	}
  }
	if (millis() >= endTime) {
		buffer[0] ='\0';
		return;
	}

  buffer[8] = '\0';
}
*/

//Read the entire register control set from 0x00 to 0x0F
//Re-written by Wyatt Olson to use i2c_master library instead of Arduino Wire library.
void Si4703_Breakout::readRegisters(){
	uint8_t message[32];	//32 uint8_t

	//Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00. (see datasheet page 19)
	// We want to read the entire register set from 0x0A to 0x09 = 32 uint8_ts.
	twi_read_from(SI4703_ADDRESS, message, 32, TWI_STOP);
	//i2c_get_data_from_transceiver(message, 33);

	//Remember, register 0x0A comes in first so we have to shuffle the array around a bit
	uint8_t j = 1;
	for (uint8_t i = 0x0A; ; i++) { //Read in these 32 uint8_ts
		if (i == 0x10) i = 0; //Loop back to zero
		this->si4703_registers[i] = ((uint16_t) message[j++]) << 8;
		this->si4703_registers[i] |= message[j++];

		if (j == 34 || i == 0x09) break; //We're done!
	}
}

//Write the current 9 control registers (0x02 to 0x07) to the Si4703
//It's a little weird, you don't write an I2C addres
//The Si4703 assumes you are writing to 0x02 first, then increments
//Re-written by Wyatt Olson to use i2c_master library instead of Arduino Wire library.
uint8_t Si4703_Breakout::updateRegisters() {
	uint8_t message[12];		//12 uint8_ts (registers 0x02..0x07, two uint8_ts each)
	uint8_t j = 0;
	for (uint8_t i = 0x02; i <= 0x07; i++){
		message[j++] = this->si4703_registers[i] >> 8;
		message[j++] = this->si4703_registers[i] & 0xFF;
	}
	twi_write_to(SI4703_ADDRESS, message, 12, TWI_BLOCK, TWI_STOP);
	return 1;
}

//Seeks out the next available station
//Returns the freq if it made it
//Returns zero if failed
uint16_t Si4703_Breakout::seek(uint8_t seekDirection){
  readRegisters();
  //Set seek mode wrap bit
  si4703_registers[POWERCFG] |= (1<<SKMODE); //Allow wrap
  //si4703_registers[POWERCFG] &= ~(1<<SKMODE); //Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first
  if(seekDirection == SEEK_DOWN) si4703_registers[POWERCFG] &= ~(1<<SEEKUP); //Seek down is the default upon reset
  else si4703_registers[POWERCFG] |= 1<<SEEKUP; //Set the bit to seek up

  si4703_registers[POWERCFG] |= (1<<SEEK); //Start seek
  updateRegisters(); //Seeking will now start

  //Poll to see if STC is set
  while(1) {
    readRegisters();
    if((si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
  }

  readRegisters();
  uint16_t valueSFBL = si4703_registers[STATUSRSSI] & (1<<SFBL); //Store the value of SFBL
  si4703_registers[POWERCFG] &= ~(1<<SEEK); //Clear the seek bit after seek has completed
  updateRegisters();

  //Wait for the si4703 to clear the STC as well
  while(1) {
    readRegisters();
    if( (si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
  }

  if(valueSFBL) { //The bit was set indicating we hit a band limit or failed to find a station
    return(0);
  }

	//After a seek has completed, set CHANNEL register with the newly seek'd channel (from READCHAN).
  si4703_registers[CHANNEL] &= ~0x03FF;	//Clear 10 LSB in CHANNEL
  si4703_registers[CHANNEL] |= (si4703_registers[READCHAN] & 0x03FF);

  updateRegisters();

  return getChannel();
}
