/*
 * Driver to co-ordinate ADC and latch on the OLS Scope expansion wing
 */

#include <avr/io.h>
#include <avr/delay.h>

#define ADC_START_CONVERSION	PINB0
#define LATCH_LOAD				PINB1

int main (void){
	//Set outputs; ADC start conversion is active low, so set it high to start.
	DDRB |= _BV(LATCH_LOAD) | _BV(ADC_START_CONVERSION);
	PORTB |= _BV(ADC_START_CONVERSION);

	//Main program loop
	while (1){
		//Start a conversion by pulsing /WR low
		PORTB &= ~_BV(ADC_START_CONVERSION);
		_delay_us(0.6);	//tWR min 600ns
		PORTB |= _BV(ADC_START_CONVERSION);
		
		_delay_us(1.3);	//tI max 1300ns
		
		//Latch outputs by pulsing latch load high; we also reset start conversion at this time
		PORTB |= _BV(LATCH_LOAD);
		_delay_us(0.03);	//tW min of 24ns; give it 30 for safety
		PORTB &= ~_BV(LATCH_LOAD);
		
		_delay_us(0.5);	//tP max 500ns
	}
}
