#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

/*
 * A flickering candle driver.  Run a number of LEDs off of PORTB0 - PORTB4, 
 * using transistors as a switch.  Recommended at least two chains (in parallel),
 * the larger one of amber LEDs being about twice as long as the shorter one of red
 * LEDs.  Optionally you can have additional chains, of yellow, orange, red, or white
 * LEDs.  PORTBx is connected to the base of each transistor to switch it.
 */

//Period
#define P 1000
//Number of pins, starting at PORTB1.  Max of 4.
#define NUM 3

int main (void){
	uint16_t a[NUM];	//Accumulated value
	uint8_t s[NUM];		//Step; assigned randomly
	uint8_t d[NUM];		//Direction
	
	volatile uint8_t *ports[NUM];
	uint8_t pins[2];
	
	for (uint8_t i = 0; i < NUM; i++){
		a[i] = P / 2;
		d[i] = 0;
		ports[i] = &PORTB;
		pins[i] = i;	//Change this to i if we start counting from B0, instead of B1
	}

	pwm_init(ports, pins, NUM, P);

	//Main program loop
	while (1){
		for (uint8_t i = 0; i < NUM; i++){
			d[i] = (i == 0 ? 2 : (i == 1 ? 3 : (i == 2 ? 5 : (i == 3 ? 7 : i * 4 + 1))));
			s[i] = random() & 0x7F;	//The larger the and'ed value, the more flicker
			a[i] += s[i] * d[i];
			
			//Keep accumulated values within bounds
			if (a[i] < 10 || a[i] > P - 10) a[i] = P / 2;
			
			//Update the PWM
			pwm_set_phase(i, a[i]);
		}
		
		_delay_ms(50);
	}
}

