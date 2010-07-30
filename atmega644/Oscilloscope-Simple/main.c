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

//#include "lib/sd-card/sd_raw.h"
//#include "lib/sd-card/sd_raw_config.h"
//#include "lib/sd-card/sd-reader_config.h"

#define BUFFER_SIZE 768

uint32_t times[BUFFER_SIZE];
uint8_t states[BUFFER_SIZE];


void record(){
	//Show record light
	PORTB |= (1 << PORTB1);
	
	_delay_ms(50);
	
	//Reset timer to zero
	timer_init();
	
	//Data which will be inserted; first 4 bytes are time, fifth is value.
	uint8_t state = 0x0;
	uint8_t last_state = 0x0;
	uint32_t time;
	
	uint16_t counter = 0;
	
	//Press record again to stop
	while (counter < BUFFER_SIZE){
		state = PINA & 0x7; //Only look at bottom 3 bits
		if (state != last_state){
			time = timer_micros();
			times[counter] = time;
			states[counter] = state;
			last_state = state;
			counter++;
		}
	}
	
	//Turn off record light, and turn on dump light
	PORTB &= ~(1 << PORTB1);
	PORTB |= (1 << PORTB0);
	
	//Dump all data which has arrived
	for (int i = 0; i < counter; i++){
		serial_write_c((times[i] >> 24) & 0xFF);
		serial_write_c((times[i] >> 16) & 0xFF);
		serial_write_c((times[i] >> 8) & 0xFF);
		serial_write_c((times[i]) & 0xFF);
		serial_write_c(states[i]);
	}
	
	PORTB &= ~(1 << PORTB0);
}



int main (void){
	//Do setup here

	//Init subsystems
	serial_init_b(9600);

	//We read the input from Port A
	DDRA = 0x0;
	//PORTA = 0xFF; //TODO debugging, use pullups
	
	//Pin D6 is (with pullup) for record start / stop.
	DDRD &= ~(1 << DDD6);
	PORTD |= (1 << DDD6);
	
	//Set pin 1 (PB0) and pin 2 (PB1) to output mode; these 
	// are the heartbeat / dump and record lights respectively.
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
			_delay_ms(100);
		}
		
		_delay_ms(100);
		
		PORTB ^= (1 << PINB0);
	}
}
