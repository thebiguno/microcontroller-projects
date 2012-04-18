#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
//#include "lib/mp3/mp3.h"
#include "lib/i2c/i2c_master.h"
#include "lib/serial/serial.h"

#define ADDRESS 0x10

//using namespace digitalcave;

static uint8_t message[8]; //Temporary array used for i2c communications

int main (void){

	//Init hardware
//	MP3 mp3 (&PORTB, 2, &PORTB, 1, &PORTB, 3);
	serial_init_b(9600);

	serial_write_s("Init\n\r");

	DDRD |= _BV(2);	//Set reset pin to output mode
	DDRC |= _BV(4);	//Set SDA to output mode
	PORTC &= ~_BV(4);	//A low SDA indicates a 2-wire interface
	PORTD &= ~_BV(2);	//Put Si4703 into reset mode
	_delay_ms(10); //Some delays while we allow pins to settle
	PORTD |= ~_BV(2);	//Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
	_delay_ms(10); //Allow Si4703 to come out of reset
	DDRC ^= ~_BV(4);	//Reset SDA back; it will be used only in i2c now

	serial_write_s("Device reset\n\r");

	i2c_master_init(400);
	sei();

	serial_write_s("i2c setup complete\n\r");

	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x16;
	message[2] = _BV(6) | _BV(2) | _BV(0); //6 == Disable external ready pin, 2 == 2g, 0 == measurement mode
	i2c_start_transceiver_with_data(message, 3);
	

	for (;;){
		_delay_ms(1000);
//		mp3.hitPlayPause();
		
//		mp3.hitVolumeDown();
	}
}

