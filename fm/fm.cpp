/*
 * Library for Sparkfun's Si4703 breakout board http://www.sparkfun.com/products/10344.  
 * Adapted from Sparkfun library.
 */

#include "fm.h"

using namespace digitalcave;

FM::FM(volatile uint8_t *resetPort, uint8_t resetPin){
	serial_init_b(9600);
	serial_write_s("Testing\n\r");

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

	serial_write_s("Init i2c...");
	//Configure the I2C connection
	i2c_master_init(100); //Now that the unit is reset and I2C inteface mode, we need to begin I2C
	serial_write_s(" Done\n\r");

	//Configure the si4703 to use the crystal
	readRegisters(); //Read the current register set
	this->si4703_registers[0x07] = 0x8100; //Enable the oscillator, from AN230 page 9, rev 0.61 (works)
	updateRegisters(); //Update
	_delay_ms(500); //Wait for clock to settle - from AN230 page 9

	//Power up the si4703 with our default settings
	//(No Mute, RDS Enabled, 200kHz channel spacing, lowest volume
	readRegisters(); //Read the current register set
	this->si4703_registers[POWERCFG] = 0x4001; //Enable the IC
	this->si4703_registers[SYSCONFIG1] |= (1<<RDS); //Enable RDS

	this->si4703_registers[SYSCONFIG2] &= ~(1<<SPACE1 | 1<<SPACE0) ; //Force 200kHz channel spacing for USA

	this->si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
	this->si4703_registers[SYSCONFIG2] |= 0x0001; //Set volume to lowest
	updateRegisters(); //Update

	//Give the si4703 enough time to power up.
	_delay_ms(110); //Max powerup time, from datasheet page 13
	
	readRegisters();
}

//Seeks out the next available station
//Returns the freq if it made it
//Returns zero if failed
uint16_t FM::seek(uint8_t seekDirection){
  readRegisters();
  //Set seek mode wrap bit
  si4703_registers[POWERCFG] |= (1<<SKMODE); //Allow wrap
  //si4703_registers[POWERCFG] &= ~(1<<SKMODE); //Disallow wrap - if you disallow wrap, you may want to tune to 87.5 first
  if(seekDirection) si4703_registers[POWERCFG] |= _BV(SEEKUP); //Seek down is the default upon reset
  else si4703_registers[POWERCFG] &= ~_BV(SEEKUP); //Set the bit to seek up

  si4703_registers[POWERCFG] |= (1<<SEEK); //Start seek
  updateRegisters(); //Seeking will now start

  //Poll to see if STC is set
  while(1) {
    readRegisters();
    if((si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
  }

  readRegisters();
  int valueSFBL = si4703_registers[STATUSRSSI] & (1<<SFBL); //Store the value of SFBL
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
return getChannel();
}

//Reads the current channel from READCHAN
//Returns a number like 973 for 97.3MHz
int FM::getChannel() {
  readRegisters();
  int channel = si4703_registers[READCHAN] & 0x03FF; //Mask out everything but the lower 10 bits
  //Freq(MHz) = 0.100(in Europe) * Channel + 87.5MHz
  //X = 0.1 * Chan + 87.5
  channel += 875; //98 + 875 = 973
  return(channel);
}


void FM::setChannel(uint16_t channel){
  //Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
  //97.3 = 0.2 * Chan + 87.5
  //9.8 / 0.2 = 49
  int newChannel = channel * 10; //973 * 10 = 9730
  newChannel -= 8750; //9730 - 8750 = 980
  newChannel /= 10; //980 / 10 = 98

  //These steps come from AN230 page 20 rev 0.5
  readRegisters();
  this->si4703_registers[CHANNEL] &= 0xFE00; //Clear out the channel bits
  this->si4703_registers[CHANNEL] |= newChannel; //Mask in the new channel
  this->si4703_registers[CHANNEL] |= (1<<TUNE); //Set the TUNE bit to start
  updateRegisters();

  //delay(60); //Wait 60ms - you can use or skip this delay

  //Poll to see if STC is set
  while(1) {
    readRegisters();
    if( (this->si4703_registers[STATUSRSSI] & (1<<STC)) != 0) break; //Tuning complete!
  }

  readRegisters();
  this->si4703_registers[CHANNEL] &= ~(1<<TUNE); //Clear the tune after a tune has completed
  updateRegisters();

  //Wait for the si4703 to clear the STC as well
  while(1) {
    readRegisters();
    if( (this->si4703_registers[STATUSRSSI] & (1<<STC)) == 0) break; //Tuning complete!
  }
}

void FM::setVolume(uint8_t volume){
  readRegisters(); //Read the current register set
  if(volume < 0) volume = 0;
  if (volume > 15) volume = 15;
  this->si4703_registers[SYSCONFIG2] &= 0xFFF0; //Clear volume bits
  this->si4703_registers[SYSCONFIG2] |= volume; //Set new volume
  updateRegisters(); //Update
}

/*
* Send the register values in the local copy to the si4703
*/
uint8_t FM::updateRegisters(){
	serial_write_s("Writing...");
	uint8_t message[13];		//12 bytes (registers 0x02..0x07, two bytes each), plus byte 0 as address / direction
	message[0] = SI4703_ADDRESS << 1 | I2C_WRITE;
	uint8_t j = 1;
	for (uint8_t i = 0x02; i <= 0x07; i++){
		message[j++] = this->si4703_registers[i] >> 8;
		message[j++] = this->si4703_registers[i] & 0xFF;
	}
	i2c_start_transceiver_with_data(message, 13);
	serial_write_s("Done\n\r");
	return 0;
}

/*
* Copy the register values in the si4703 to the local array
*/
void FM::readRegisters(){
	serial_write_s("Reading...\n\r");
	uint8_t message[33];	//32 bytes, plus byte 0 as address / direction
	message[0] = SI4703_ADDRESS << 1 | I2C_READ;

	//Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00. (see datasheet page 19)
	// We want to read the entire register set from 0x0A to 0x09 = 32 bytes.
	i2c_start_transceiver_with_data(message, 33);
	i2c_get_data_from_transceiver(message, 33);
	
	//Remember, register 0x0A comes in first so we have to shuffle the array around a bit
	uint8_t j = 1;
	for (uint8_t i = 0x0A; ; i++) { //Read in these 32 bytes
		if (i == 0x10) i = 0; //Loop back to zero
		this->si4703_registers[i] = ((uint16_t) message[j++]) << 8;
		this->si4703_registers[i] |= message[j++];

		if (j == 34 || i == 0x09) break; //We're done!
	}
	
	char temp[10];
	
	for (uint8_t i = 0; i < 16; i++){
		serial_write_s(itoa(i, temp, 16));
		serial_write_s("\t");
		serial_write_s(itoa(this->si4703_registers[i], temp, 16));
		serial_write_s("\n\r");
	}
	
	serial_write_s("Done\n\r");
}