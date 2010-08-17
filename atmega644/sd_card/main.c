/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

#include "lib/sd-card/sd_raw.h"
#include "lib/sd-card/sd_raw_config.h"
#include "lib/sd-card/sd-reader_config.h"

int main (void){
	//Do setup here
	DDRA = 0xFF;
	PORTA = 0xFF;	

	serial_init_b(9600);
	
	_delay_ms(500);
	serial_write_s("Starting...\n");
	
	//Give the programmer time to finish
	_delay_ms(5000);

//
	int ret;
	char retStr[10];


	//Init SD card
	if((ret = sd_raw_init()) != 1) {
		itoa(ret, retStr, 16);
		while (1) {serial_write_s(" init error\n"); serial_write_s(retStr); _delay_ms(500);}
	};

	unsigned char data[256];
	for (int i = 0; i < 256; i++){
		data[i] = i;
	}

	serial_write_s("Starting to write...\n");
	if((ret = sd_raw_write(100, data, 256)) != 1) {
		itoa(ret, retStr, 16);
		while (1) {serial_write_s(" write error\n"); serial_write_s(retStr); _delay_ms(500);}
	}
	
	serial_write_s("Starting to read...");
	unsigned char read_data[2];
	read_data[1] = '\0';
	for (int i = 100; i < 300; i++){
		sd_raw_read(i, read_data, 1);
		serial_write_s((char*) read_data);
		serial_write_s("\n");
	}
//*/
	DDRB = 0xFF;
	PORTB = 0xFF;


	//Enable MUX to write to card
	DDRD |= (1 << DDD6);
	PORTD |= (1 << PORTD6);
	
	//Main program loop
	while (1){
		PORTA = ~PORTA;

		serial_write_s("Loop...\n");
		
		_delay_ms(1000);
	}
}

