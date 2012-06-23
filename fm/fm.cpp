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
	i2c_master_init(400); //Now that the unit is reset and I2C inteface mode, we need to begin I2C
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
}

/*
* Send the register values in the local copy to the si4703
*/
uint8_t FM::updateRegisters(){
	serial_write_s("Writing...");
	uint8_t message[13];		//12 bytes (registers 0x02..0x07, two bytes each), plus byte 0 as address / direction
	message[0] = SI4703_ADDRESS << 1 | I2C_WRITE;
	for (uint8_t i = 0x02; i <= 0x07 * 2; i += 2){
		message[i - 1] = this->si4703_registers[i] >> 8;
		message[i] = this->si4703_registers[i] & 0xFF;
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
	
	char temp[10];
	
	for (uint8_t i = 1; i <= 32; i++){
		serial_write_s(itoa(i, temp, 16));
		serial_write_s("\t");
		serial_write_s(itoa(message[(i + 1) / 2], temp, 16));
		serial_write_s("\n\r");
	}
	
	//Remember, register 0x0A comes in first so we have to shuffle the array around a bit
	for (uint8_t i = 0x0A; ; i++) { //Read in these 32 bytes
		if (i == 0x10) i = 0; //Loop back to zero
		this->si4703_registers[i] = message[i * 2 + 1] << 8;
		this->si4703_registers[i] |= message[i * 2 + 2];
		
		if (i == 0x09) break; //We're done!
	}
	
	serial_write_s("Done\n\r");
}