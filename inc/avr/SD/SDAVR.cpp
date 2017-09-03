/*
 * Much of this code was inspired by https://luckyresistor.me/cat-protector/software/sdcard-2/
 * and http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
 */
#include "SD.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <dcutil/spi.h>

using namespace digitalcave;

#define Cmd0_GoIdleState           0x00
#define Cmd8_SendIfCond            0x48
#define Cmd16_SetBlockLength       0x10
#define Cmd17_ReadSingleBlock      0x11
#define Cmd55_ApplicationCommand   0x37
#define Cmd58_ReadOCR              0x3a

#define ACmd41_SendOpCond          0x29

#define R1_IllegalCommand          0x04
#define R1_InIdleState             0x01

SD::SD(volatile uint8_t* port_cs, uint8_t pin_cs) :
	port_cs(port_cs),
	pin_cs(pin_cs),
	block(0),
	position(0)
{
	*(this->port_cs - 0x01) |= this->pin_cs;	// Set CS DDR to output
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);     // Set MOSI and SCK output
	SPCR = (1<<SPE)|(1<<MSTR); //Enable SPI, Master

	this->status = this->init();
}

SD::~SD() {
}

void SD::select() {
	*(this->port_cs) &= ~(this->pin_cs);
}

void SD::deselect() {
	*(this->port_cs) |= this->pin_cs;
}

void SD::slow_clock() {
	SPCR |=  (1<<SPR1) | (1<<SPR0);   // fck/128 = 125Khz
	SPSR &= ~(1<<SPI2X);              // No doubled clock frequency
}

void SD::fast_clock() {
	SPCR &= ~((1<<SPR1) | (1<<SPR0)); // fck/4 = 4MHz
	SPSR |=  (1<<SPI2X);              // doubled clock frequency = 8MHz
}

uint8_t SD::transfer(uint8_t b) {
	SPDR = b;
	while(!(SPSR & (1<<SPIF))) {}
	return SPDR;
}

/*
 * Sends a command.
 * bit 7 of cmd is always 0
 * bit 6 of cmd indicates if a (1) 5 byte or (0) 1 byte response is expected.
 */
uint8_t SD::command(uint8_t cmd, uint32_t arg, uint32_t* resp) {
	// commands have the following structure
	// [47] start bit = 0
	// [46] transmission bit = 1
	// [45-40] command
	// [39-8] arguement
	// [7-1] crc
	// [0] end bit = 1

	this->select();

	// crc is 7 bits + 1 stop bit (always 1)
	uint8_t crc = 0xff;
	// the CRC is important for these two commands and ignored for all others
	if (cmd == Cmd0_GoIdleState) { crc = 0x95; }
  else if (cmd == Cmd8_SendIfCond) { crc = 0x87; }

	uint8_t result;
	uint8_t responseValues[4];
	this->transfer((cmd & 0x3f) | 0x40); // 6 bit command | transmission bit
	this->transfer(arg>>24);
	this->transfer(arg>>16);
	this->transfer(arg>>8);
	this->transfer(arg);
	this->transfer(crc);
	// read until start bit is 0, up to 16 times
	for (uint8_t i = 0; ((result = this->transfer(0xff)) & 0x80) && i < 0x10; i++);

	if ((cmd & 0xc0) == 0x40) {
		// bit 6 is set, so read 4 more bytes as the response
		// Read the next 4 bytes
		for (uint8_t i = 0; i < 4; ++i) {
			responseValues[i] = this->transfer(0xff);
		}
		if (resp != 0) {
			*resp = (((uint32_t)responseValues[0]) << 24) |
			(((uint32_t)responseValues[1]) << 16) |
			(((uint16_t)responseValues[2]) << 8) |
			(responseValues[3]);
		}
	}
	this->deselect();
	return result;
}

/* Initialize the card; return 1 if successful, or 0 if unsuccessful */
uint8_t SD::init() {
	uint8_t i;
	uint8_t type;
	uint8_t result;
	uint32_t resp;

	this->slow_clock();

	// send clock pulses for 80 cycles
	// it's this clocking that switches the card from SDIO to SPI mode
	this->select();
	for (i=0; i<10; i++) {
		this->transfer(0xff);
	}

	// send clock pulses for 16 more cycles
	this->deselect();
	this->transfer(0xff);
	this->transfer(0xff);

	// try up to 10 times to reset the card and put it into idle state
	for (i = 0; i < 10; i++) {
		result = this->command(Cmd0_GoIdleState, 0, 0);
		if (result == R1_InIdleState) {
			i = 0xf;
		}
	}
	if (i < 0xf) {
		return 0; // card did not reset and go into idle state
	}

	// check the SD card version
	result = this->command(Cmd8_SendIfCond, 0x01aa, &resp);
	if ((result & R1_IllegalCommand) == R1_IllegalCommand) {
		// card doesn't understand Cmd8
		type = 1; // SD 1.0 or MMC
	} else {
		// card understands Cmd8
		if ((resp & 0xff) != 0xaa) {
			return 0;
		}
		type = 2; // SD 2.0 SDHC, SDCX
	}

	// try up to 10 times to initialize the card and have it go idle
	for (i = 0; i < 10; i++) {
		// flag the next command as an application command
		result = this->command(Cmd55_ApplicationCommand, 0, 0);
		// have the card send it's OCR
		uint32_t arg = type == 2 ? 0x40000000 : 0x00; // enable HCS flag for type 2 cards
		result = this->command(ACmd41_SendOpCond, arg, &resp);
		if (result == R1_InIdleState) {
			i = 0xf;
		}
	}
	if (i < 0xf) {
		return 0; // card did not go idle in 10 attempts
	}

	// read the operating conditions
	result = this->command(Cmd58_ReadOCR, 0, &resp);
  if ((resp & 0x300000) == 0) { // either bit 20 or 21 must be set to indicate 3.3V
		return 0; // card does not allow 3.3V
	}

	// set the block length to 512 bytes
	result = this->command(Cmd16_SetBlockLength, 0x200, 0);

	this->deselect();
	this->fast_clock();
	return 1;
}

void SD::setBlock(uint32_t block) {
	this->block = block;
	this->position = 0;
}

uint16_t SD::skip(uint16_t len) {
	if (len > 512 || 512 - len < this->position) {
		len = 512 - this->position;
	}
	this->position += len;
	return len;
}

/*
 * Using this method is a really bad idea
 */
uint8_t SD::read(uint8_t* a){
	uint8_t result;
	uint16_t read = this->read(&result, 1);
	return (read == 1) ? result : 0;
}

uint16_t SD::read(uint8_t* a, uint16_t len){
	if (status > 0) return 0;

	if (len > 512 || 512 - len < this->position) {
		len = 512 - this->position;
	}
	uint16_t count = 0;
	uint8_t b = 0;
	uint32_t resp;

  uint8_t result = this->command(Cmd17_ReadSingleBlock, this->block, &resp);
	if (result != 0) {
		this->deselect();
		return 0;
	}

	for (uint8_t i = 0; i < 16; i++){
		b = this->transfer(0xff);
		if (b == 0xff) {
			_delay_ms(100);
		} else {
			break;
		}
		if (b != 0xfe) {
			// last byte read was not the data token
			// either an error token was read or the card was busy
			this->deselect();
			return 0;
		}
	}

	// read all 512 bytes, filling the array with the requested bytes
	for (uint16_t i = 0; i < 512; i++) {
		b = this->transfer(0xff);
		if (i == this->position && count < (len - 1)) {
			a[count++] = b;
		}
	}

	// read the CRC bytes
	b = this->transfer(0xff);
	b = this->transfer(0xff);

	this->deselect();

	return count;
}

// TODO for now just read-only
uint8_t SD::write(uint8_t b) {
	return 0;
}
