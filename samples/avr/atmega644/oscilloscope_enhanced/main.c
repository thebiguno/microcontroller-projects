/*
 * Simple Oscilloscope.  Reads digital values on port A, and logs to SD card.
 * After completing the reading, it dumps to serial port.
 *
 * Board setup:
 *  -VCC / GND
 *  -Probes on PORTA
 *  -SD card on SPI (MOSI / MISO / SS / SCK)
 *  -Record button on pin D6; dump button on D5
 *  -Serial output connected
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

#include "lib/timer/timer.h"

#include "lib/sd-card/sd_raw.h"
#include "lib/sd-card/sd_raw_config.h"
#include "lib/sd-card/sd-reader_config.h"

#define SD_START_ADDRESS 1024
#define SD_LAST_DATA_LENGTH_ADDRESS 512
#define BUFFER_SIZE 128

uint32_t times[BUFFER_SIZE];
uint8_t states[BUFFER_SIZE];

//Temp string for output
char temp[32]; 
	


uint8_t record(){
	serial_write_s("Starting data dump\n");

	//Show record light
	PORTB |= (1 << PORTB1);
	
	//Init SD card
	if(!sd_raw_init()) {
		serial_write_s("SD init error\n");
		return 0;
	}
	
	//Overwrite last log length
	uint32_t sd_address = SD_START_ADDRESS;
	uint8_t four_bytes[4];
	four_bytes[0] = (sd_address >> 24) & 0xFF;
	four_bytes[1] = (sd_address >> 16) & 0xFF;
	four_bytes[2] = (sd_address >> 8) & 0xFF;
	four_bytes[3] = (sd_address) & 0xFF;
	
	if (!sd_raw_write(SD_LAST_DATA_LENGTH_ADDRESS, (uint8_t*) four_bytes, 4)){
		serial_write_s("Could not erase last data size\n");
		PORTB &= ~(1 << PORTB1);
		return 0;
	}
	
	_delay_ms(100);
	
	//Reset timer to zero
	timer_init();
	
	//Data which will be inserted; first 4 bytes are time, fifth is value.
	uint8_t reading[5];
	uint32_t time = 0x0;
	uint8_t value = 0x0;
	uint8_t last_value = 0x0;
	
	//Press record again to stop
	while (PIND & (1 << PIND6)){
		value = PINA;
		if (1 == 1){  //TODO this is for testing
		//if (value != last_value){
			time = timer_micros();
			reading[0] = (time >> 24) & 0xFF;
			reading[1] = (time >> 16) & 0xFF;
			reading[2] = (time >> 8) & 0xFF;
			reading[3] = (time) & 0xFF;
			reading[4] = value;
			serial_write_s(itoa(reading[4], temp, 16));
			serial_write_c('\n');
			if (!sd_raw_write(sd_address++, (uint8_t*) reading, 5)){
				serial_write_s("Could not write value to address ");
				serial_write_s(ltoa(sd_address, temp, 16));
				serial_write_c('\n');
				PORTB &= ~(1 << PORTB1);
				return 0;			
			}
			last_value = value;
		}
	}
	
	//Record how far we have gone so that we can send it back later
	four_bytes[0] = (sd_address >> 24) & 0xFF;
	four_bytes[1] = (sd_address >> 16) & 0xFF;
	four_bytes[2] = (sd_address >> 8) & 0xFF;
	four_bytes[3] = (sd_address) & 0xFF;
	
	if (!sd_raw_write(SD_LAST_DATA_LENGTH_ADDRESS, four_bytes, 4)){
		serial_write_s("Could not record length to end of data\n");
		PORTB &= ~(1 << PORTB1);
		return 0;
	}
	
	
	//Turn off record light
	PORTB &= ~(1 << PORTB1);
	
	return 1;
}

uint8_t dump(){
	//Show record light
	PORTB |= (1 << PORTB0);
	
	//Init SD card
	if(!sd_raw_init()) {
		serial_write_s("SD init error\n");
		return 0;
	}
	
	uint8_t four_bytes[4];	
	if (!sd_raw_read(SD_LAST_DATA_LENGTH_ADDRESS, four_bytes, 4)){
		serial_write_s("Could not read last data size\n");
		PORTB &= ~(1 << PORTB0);
		return 0;
	}
	
	//Overwrite last log length
	uint32_t sd_end_address = 0;
		
	sd_end_address = four_bytes[0];
	sd_end_address |= (((uint32_t) four_bytes[1]) << 8);
	sd_end_address |= (((uint32_t) four_bytes[2]) << 16);
	sd_end_address |= (((uint32_t) four_bytes[3]) << 24);
	
	
	serial_write_s("Last end of data position: ");
	serial_write_s(ltoa(sd_end_address, temp, 16));
	serial_write_c('\n');
	
	_delay_ms(100);
	
	//Data which is read; first 4 bytes are time, fifth is value.
	uint8_t reading[5];
	
	//Press record again to stop
	uint8_t five_bytes[5] = {'\0'};
	for (int i = SD_START_ADDRESS; i < sd_end_address; i+=5){
		if (!sd_raw_read(i, (uint8_t*) reading, 5)){
			serial_write_s("Could not read last data size\n");
			PORTB &= ~(1 << PORTB0);
			return 0;
		}
		
		uint32_t time = five_bytes[0];
		time |= (((uint32_t) five_bytes[1]) << 8);
		time |= (((uint32_t) five_bytes[2]) << 16);
		time |= (((uint32_t) five_bytes[3]) << 24);
		uint8_t value = five_bytes[4];
		
		serial_write_s(ltoa(time, temp, 10));
		serial_write_c(',');
		serial_write_c((char) value);
	}
		
	//Turn off dump light
	PORTB &= ~(1 << PORTB0);
	
	return 1;
}





int main (void){
	//Do setup here

	//Init subsystems
	serial_init_b(9600);

	//We read the input from Port A
	DDRA = 0x0;
	
	//Pins D5 and D6 are inputs (with pullups) for record / dump respectively.
	DDRD &= ~((1 << DDD5) | (1 << DDD6));
	PORTD |= (1 << DDD5) | (1 << DDD6);
	
	//Set pin 1 (PB0) and pin 2 (PB1) to output mode; these 
	// are the heartbeat and record light respectively.
	DDRB |= (1 << DDB0) | (1 << DDB1);
		
	//Main program loop - whenever there is data in the buffer, write it
	while (1){
		if (!(PIND & (1 << PIND6))){
			PORTB &= ~(1 << PINB0);
			record();
			
			//Wait for a bit to let buffers clear, and to 
			// debounce the button so you don't lose everything.
			// (Yes, there are better ways to debounce, but at
			// this point there is no rush).
			_delay_ms(500);
		}
		if (!(PIND & (1 << PIND5))){
			dump();
			
			//Wait for a bit to let buffers clear, and to 
			// debounce the button so you don't lose everything.
			// (Yes, there are better ways to debounce, but at
			// this point there is no rush).
			_delay_ms(500);
		}
		
		_delay_ms(100);
		
		PORTB ^= (1 << PINB0);
	}
}

//ISR(PCINT1_vect){
/*
void old(){
	//Read the data bits into state, and record micros
	if (head + 1 == tail || (head + 1 == BUFFER_SIZE && tail == 0)){
		//If there is a buffer overrun, light the LED.
		PORTB = (1 << PORTB1);
	}
	else {
		times[head] = timer_micros();
		states[0][head] = PINC & 0xF;
		head++;
		if (head >= BUFFER_SIZE) head = 0;
		
		//Once the buffer is cleared, clear the error
		PORTB = 0x0;
	}
}

*/
