#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/Button/Buttons.h"

using namespace digitalcave;

#define LANE_COUNT			3

#define START_BUTTON		_BV(PORTB0)
#define STOP_BUTTON			_BV(PORTB1)
#define RESET_BUTTON		_BV(PORTB4)

volatile uint16_t time;
volatile uint16_t finish_times[LANE_COUNT];

uint8_t number_to_segmented_digit(uint16_t value, uint8_t digit){
	return 0x00;	//TODO
}

/*
 * Display the given non-zero times, or fall back to default_value if the time is zero. (Pass zero as the default
 * if you want to show zero times.)
 */
void display_times(volatile uint16_t* times, volatile uint16_t default_value){
	static uint8_t digit = 0;

	for (uint8_t b = 0; b < LANE_COUNT; b++){
		SPDR = number_to_segmented_digit(times[b], digit);
		while (!(SPSR & _BV(SPIF)));
	}
	PORTD = 0x00;
	PORTB &= ~_BV(PORTB2);
	PORTB |= _BV(PORTB2);
	PORTD = ~_BV(digit);
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
	timer_init();

	DDRB |= _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB5);
	//SPCR |= _BV(SPI2X);
	SPCR = _BV(SPE) | _BV(MSTR);

	Buttons b(&PORTB, START_BUTTON | STOP_BUTTON | RESET_BUTTON, 8, 8, 0, 0);

	DDRD = 0x0F;

	while (1) {
		uint8_t pressed = 0x00;
		
		for(uint8_t i = 0; i < LANE_COUNT; i++){
			finish_times[i] = 0;
		}
		
		while(!(pressed & START_BUTTON)){
			b.sample();
			pressed = b.pressed();
			
			display_times(finish_times, 0);
		}
		
		time = 0;	//Reset timer
		
		while(!(pressed & RESET_BUTTON)){
			display_times(finish_times, time);
			
			b.sample();
			pressed = b.pressed();
		}
	}
}

ISR(TIMER1_COMPA_vect){
	time++;
}

//TODO Add pin change listener for finish times
