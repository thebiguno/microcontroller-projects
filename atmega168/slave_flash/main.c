#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lib/analog/analog.h"
#include "lib/serial/serial.h"

#define AMBIENT_VALUE 	5
#define FLASH_VALUE 	40
#define DEBUG 			0
#define WAIT_LOOP		50000

int main (void){
	if (DEBUG) {
		serial_init_b(9600);
	}
	
	uint8_t pins[1];
	pins[0] = 2;
	analog_init(pins, 1);
		
	char temp[64];
	uint16_t ambient_analog = 0, flash_analog = 0;

	//Get initial reading to figure out ambient light
	analog_read_a(&ambient_analog);

	//PINC5 is output
	DDRC |= _BV(PINC5);

	//Main program loop
	while (1){
		analog_read_a(&flash_analog);
		
		if (flash_analog - FLASH_VALUE > ambient_analog){
			
			//Read the 'post-pre-flash' return to ambient
			uint16_t i = 0;
			while (i < WAIT_LOOP && flash_analog - AMBIENT_VALUE <= ambient_analog){
				analog_read_a(&flash_analog);	
				i++;
			}
		
			i = 0;
			while (i < WAIT_LOOP){
				i++;
				analog_read_a(&flash_analog);
				if (flash_analog - FLASH_VALUE > ambient_analog){
					PORTC |= _BV(PINC5);
					_delay_ms(1);
					PORTC &= ~_BV(PINC5);
					i = WAIT_LOOP; //break for
					
					if (DEBUG){
						sprintf(temp, "flash: %d, %d\n\r", ambient_analog, flash_analog);
						serial_write_s(temp);
					}
					
					//Don't re-flash
					_delay_ms(200);
				}
			}
			
			analog_read_a(&ambient_analog);
		}
	}
}
