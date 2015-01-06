#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/twi/twi.h"
#include "lib/magnetometer/magnetometer.h"

#define MAGNETOMETER_ADDRESS 0x1E

static volatile uint8_t ready = 0x00;

int main(){
	DDRB |= _BV(PORTB1);
	
	serial_init_b(9600);

	magnetometer_init();

	//Main program loop
	while (1){
		double heading = magnetometer_read_heading();
		//int16_t data[3];
		//magnetometer_read_raw(data);
		
		char temp[100];
		sprintf(temp, "Heading: %2.3f\n\r", heading);
		//sprintf(temp, "%d\t%d\t%d\n\r", data[0], data[1], data[2]);
		serial_write_s(temp);

		PORTB ^= _BV(PORTB1);
		_delay_ms(100);
	}
}

//ISR(PCINT1_vect){
//	if (PINB & _BV(PINB5)){
//		ready = 0x01;
//	}
//}