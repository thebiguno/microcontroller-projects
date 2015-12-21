#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Teensy.h"

using namespace digitalcave;

Hd44780_Teensy::Hd44780_Teensy(uint8_t function, uint8_t pin_rs, uint8_t pin_e,
		uint8_t pin_d4, uint8_t pin_d5, uint8_t pin_d6, uint8_t pin_d7) {

	this->pin_rs = pin_rs;
	this->pin_e = pin_e;
	this->pin_d4 = pin_d4;
	this->pin_d5 = pin_d5;
	this->pin_d6 = pin_d6;
	this->pin_d7 = pin_d7;
	
	pinMode(this->pin_rs, OUTPUT);
	pinMode(this->pin_e, OUTPUT);
	pinMode(this->pin_d4, OUTPUT);
	pinMode(this->pin_d5, OUTPUT);
	pinMode(this->pin_d6, OUTPUT);
	pinMode(this->pin_d7, OUTPUT);
	
	
	delay(100);
	write_nibble(0x03, 0);
	delay(10);
	write_nibble(0x03, 0);
	delay(1);
	write_nibble(0x03, 0);
	delay(1);
	write_nibble(0x02, 0);
	delay(1);
	
	this->write_command((function & 0x0c) | 0x20);	//Function set (char size + lines)
	delayMicroseconds(60);

	this->write_command(0x08);	//Display off
	delayMicroseconds(60);
	this->write_command(0x01);	//Clear display
	delay(5);
	this->write_command(0x04);	//Entry mode set
	delay(60);
	this->write_command(0x0C);	//Display on, cursor off, cursor blink off
	this->clear();		//Clear display
}

/*
 * Writes the nibble.  Only the bottom 4 bits of b are used.
 */
void Hd44780_Teensy::write_nibble(uint8_t b, uint8_t mode){
	if (mode) digitalWriteFast(this->pin_rs, 1);	//Set RS high for data
	else digitalWriteFast(this->pin_rs, 0);	//Set RS low for command
	
	//Set data lines to specified nibble
	if (b & 0x01) digitalWriteFast(this->pin_d4, 1);
	else digitalWriteFast(this->pin_d4, 0);
	
	if (b & 0x02) digitalWriteFast(this->pin_d5, 1);
	else digitalWriteFast(this->pin_d5, 0);
	
	if (b & 0x04) digitalWriteFast(this->pin_d6, 1);
	else digitalWriteFast(this->pin_d6, 0);
	
	if (b & 0x08) digitalWriteFast(this->pin_d7, 1);
	else digitalWriteFast(this->pin_d7, 0);

	//Toggle E
	delayMicroseconds(50);
	digitalWriteFast(this->pin_e, 1);	//Set E high to clock in data
	delayMicroseconds(50);
	digitalWriteFast(this->pin_e, 0);	//Re-set E back low
	delayMicroseconds(50);
	

}

void Hd44780_Teensy::write_byte(uint8_t b) {
	write_nibble(((b & 0xF0) >> 4), 1);
	write_nibble((b & 0x0F), 1);
}

void Hd44780_Teensy::write_command(uint8_t b) {
	write_nibble(((b & 0xF0) >> 4), 0);
	write_nibble((b & 0x0F), 0);
}
