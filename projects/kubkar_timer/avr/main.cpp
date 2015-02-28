#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "lib/Button/Buttons.h"

using namespace digitalcave;

#define LANE_COUNT			3

#define START_BUTTON		_BV(PORTB0)
#define STOP_BUTTON			_BV(PORTB1)

volatile uint16_t time;
volatile uint16_t finish_times[LANE_COUNT];

uint8_t number_to_segmented_digit(uint8_t digit){
	//Mapping between digits and 7 segment values.  Since the display
	// is common anode, we need to invert the values (a high bit means off).
	// We always keep pin LED pin 3 (the colon) enabled.
	switch(digit){
		case '0':
			return 0x02;
		case '1':
			return 0x73;
		case '2':
			return 0x44;
		case '3':
			return 0x60;
		case '4':
			return 0x31;
		case '5':
			return 0xA0;
		case '6':
			return 0x80;
		case '7':
			return 0x72;
		case '8':
			return 0x00;
		case '9':
			return 0x20;
	}
	
	return 0xFF;
}

/*
 * Display the given non-zero times, or fall back to default_value if the time is zero. (Pass zero as the default
 * if you want to show zero times.)
 */
void display_times(volatile uint16_t* times, volatile uint16_t default_value){
	static uint8_t digit = 0;

	char temp[10];
	
	snprintf(temp, 6, "%04d", times[0] == 0 ? default_value : times[0]);
	for (uint8_t b = 0; b < LANE_COUNT; b++){
		SPDR = number_to_segmented_digit(temp[digit]);
		if ((b + 1) < LANE_COUNT){
			snprintf(temp, 6, "%04d", times[b + 1] == 0 ? default_value : times[b + 1]);
		}

		while (!(SPSR & _BV(SPIF)));
	}
	_delay_us(5);
	PORTD = 0x0F;
	PORTB &= ~_BV(PORTB2);
	_delay_us(100);
	PORTB |= _BV(PORTB2);
	_delay_us(100);
	PORTD = ~_BV(digit) & 0x0F;
	_delay_ms(2);

	digit++;
	if (digit >= 4) digit = 0;
}

void timer_init(){
	//Set up the timer to run at F_CPU/64 in CTC mode
	TCCR1A = 0x0;
	TCCR1B |= _BV(CS11) | _BV(CS10) | _BV(WGM12);
	
	//Set compare value to be F_CPU / 100 (with a 64 prescaler) -- fire interrupt every ten milliseconds
	OCR1A = F_CPU / 6400;
	
	//Enable compare interrupt
	TIMSK1 = _BV(OCIE1A);

	sei();
}

int main (void){
	//Set up SPI.  We need to set SS (B2) as an output before enabling master mode
	DDRB |= _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB5);
	//SPSR = _BV(SPI2X);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
	
	//Start / stop buttons
	Buttons b(&PORTB, START_BUTTON | STOP_BUTTON, 8, 8, 0, 0);
	
	//We drive the anodes of the LED modules via MOSFETs activated by PORTD0..PORTD3
	DDRD |= 0xFF;

	while (1) {
		uint8_t pressed = 0x00;
		
		for(uint8_t i = 0; i < LANE_COUNT; i++){
			finish_times[i] = 0;
		}
		
		//Wait until race starts, showing 00:00
		while(1){

			b.sample();
			pressed = b.pressed();
			
	
			display_times(finish_times, 0);
			
			if (pressed & START_BUTTON) break;
		}
		
		//Start the timer
		timer_init();
		time = 0;
		
		//Wait for the races to finish, the stop button to be pressed, or the time to run out (99.99 seconds).
		while(1){
			display_times(finish_times, time);
			
			b.sample();
			pressed = b.pressed();
			
			for(uint8_t i = 0; i < LANE_COUNT; i++){
				if (finish_times[i] == 0 && PINC & _BV(i)){	//The sensors go high when finishing
					finish_times[i] = time;
				}
			}
			
			if (pressed & STOP_BUTTON || time >= 9999){
				for(uint8_t i = 0; i < LANE_COUNT; i++){
					if (finish_times[i] == 0) finish_times[i] = time;
				}
				TCCR1B = 0x00;	//Turn off the timer
				break;
			}
			
			uint8_t all_finished = 0x01;
			for(uint8_t i = 0; i < LANE_COUNT; i++){
				if (finish_times[i] == 0) all_finished = 0x00;
			}
			if (all_finished){
				TCCR1B = 0x00;	//Turn off the timer
				break;
			}
		}
		
		//Wait for a reset (hit stop button again)
		while(1){
			display_times(finish_times, time);
			
			b.sample();
			pressed = b.pressed();
			if (pressed & STOP_BUTTON) break;
		}
		
		TCCR1B = 0x00;	//Turn off the timer
	}
}

EMPTY_INTERRUPT(TIMER1_COMPB_vect)
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
	time++;
}

//TODO Add pin change listener for finish times
