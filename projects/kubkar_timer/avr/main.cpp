#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "lib/Button/Buttons.h"

using namespace digitalcave;

#define LANE_COUNT			3

#define START_BUTTON		_BV(PORTB0)
#define STOP_BUTTON			_BV(PORTB1)

volatile uint16_t time;
uint16_t finish_times[LANE_COUNT];

inline uint8_t digit_to_segments(uint8_t digit){
	//Mapping between digits and 7 segment values.  Since the display
	// is common anode, we need to invert the values (a high bit means off).
	// We always keep pin LED pin 3 (the colon) enabled.
	switch(digit){
		case 0:
			return 0x02;
		case 1:
			return 0x73;
		case 2:
			return 0x44;
		case 3:
			return 0x60;
		case 4:
			return 0x31;
		case 5:
			return 0xA0;
		case 6:
			return 0x80;
		case 7:
			return 0x72;
		case 8:
			return 0x00;
		case 9:
			return 0x20;
	}
	
	return 0xFF;
}

inline uint8_t extract_digit(uint16_t number, uint8_t digit){
	switch(digit){
		case 3:
			return number % 10;
		case 2:
			return number % 100 / 10;
		case 1:
			return number % 1000 / 100;
		case 0:
			return number % 10000 / 1000;
	}
	
	return 0;
}

/*
 * Display the non-zero finish times, or show default_value if the time is zero. (Pass zero as the default
 * if you want to show zero times.)
 */
void display_times(uint16_t default_value){
	//To keep the digit-enabling math simpler, we reverse the logical order of 
	// the digits.  Digit 3 is the 1's place; digit 2 is the 10's place, etc.
	static uint8_t digit = 0;

	for (uint8_t i = 0; i < LANE_COUNT; i++){
		SPDR = digit_to_segments(extract_digit(finish_times[i] == 0 ? default_value : finish_times[i], digit));

		while (!(SPSR & _BV(SPIF)));
		_delay_us(10);
	}
	_delay_us(10);
	PORTD = 0x0F;					//Blank the display
	_delay_us(10);
	PORTB &= ~_BV(PORTB2);			//Falling edge of RCLK (latch) line
	_delay_us(500);
	PORTB |= _BV(PORTB2);			//Rising edge of RCLK (latch) line
	_delay_us(100);
	PORTD = ~_BV(digit) & 0x0F;		//Show the selected digit
	_delay_ms(1);

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
	Buttons b(&PORTB, START_BUTTON | STOP_BUTTON, 8, 8, 10, 10);
	
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
			pressed = b.repeat();
			
	
			display_times(0);
			
			if (pressed & START_BUTTON){	//The start button must begin as pressed...
				while ((PINB & START_BUTTON) == 0){	//... then it must be released (remember this is active low, so we loop until it goes high (released)
					display_times(0);
				}
				break;
			}
		}
		
		//Start the timer
		timer_init();
		time = 0;
		
		//Wait for the races to finish, the stop button to be pressed, or the time to run out (99.99 seconds).
		while(1){
			display_times(time);
			
			b.sample();
			pressed = b.pressed();
			
			for(uint8_t i = 0; i < LANE_COUNT; i++){
				if ((PINC & _BV(i)) && (finish_times[i] == 0)){	//The sensors go high when finishing
					finish_times[i] = time;
				}
			}
			
			if (pressed & STOP_BUTTON || (finish_times[0] && finish_times[1] && finish_times[2]) || time >= 9999){
				for(uint8_t i = 0; i < LANE_COUNT; i++){
					if (finish_times[i] == 0) finish_times[i] = time;
				}
				TCCR1B = 0x00;	//Turn off the timer
				break;
			}
		}
		
		//Wait for a reset (hit stop button again)
		while(1){
			display_times(9999);
			
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
