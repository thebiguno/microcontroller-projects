#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "lib/analog/analog.h"
#include "lib/Button/Buttons.h"

using namespace digitalcave;

#define ANALOG_THRESHOLD	100

#define LANE_COUNT			3

#define START_BUTTON		_BV(PORTB0)
#define STOP_BUTTON			_BV(PORTB1)

volatile uint16_t time;
uint16_t finish_times[LANE_COUNT];

inline uint8_t digit_to_segments(uint8_t digit){
	//Mapping between digits and 7 segment values.  Since the display
	// is common anode, we need to invert the values (a high bit means off).
	// We always keep pin LED pin 3 (the colon) disabled.
	switch(digit){
		case 0:
			return 0x0A;
		case 1:
			return 0x7B;
		case 2:
			return 0x4C;
		case 3:
			return 0x68;
		case 4:
			return 0x39;
		case 5:
			return 0xA8;
		case 6:
			return 0x88;
		case 7:
			return 0x7A;
		case 8:
			return 0x08;
		case 9:
			return 0x28;
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
	if (default_value > 9999) default_value = 9999;
	
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
	
	//Set compare value to be F_CPU / 1000 (with a 64 prescaler) -- fire interrupt every millisecond
	OCR1A = F_CPU / 64000;
	
	//Enable compare interrupt
	TIMSK1 = _BV(OCIE1A);

	sei();
}

int main (void){
	//Set up SPI.  We need to set SS (B2) as an output before enabling master mode
	DDRB |= _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB5);
	//SPSR = _BV(SPI2X);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0) | _BV(CPOL) | _BV(CPHA);
	
	//Start / stop buttons
	Buttons b(&PORTB, START_BUTTON | STOP_BUTTON, 8, 8, 10, 10);
	
	//Sensors
	uint8_t analog_pins[LANE_COUNT];
	for (uint8_t i = 0; i < LANE_COUNT; i++){
		analog_pins[i] = i;
	}
	analog_init(analog_pins, LANE_COUNT, ANALOG_AVCC);
	
	//We drive the anodes of the LED modules via MOSFETs activated by PORTD0..PORTD3
	DDRD |= 0xFF;
	
	uint16_t starting_sensor_values[LANE_COUNT];
	uint16_t sensor_values[LANE_COUNT];

	while (1) {
		uint8_t pressed = 0x00;
		
		for(uint8_t i = 0; i < LANE_COUNT; i++){
			finish_times[i] = 0;
		}
		
		//Wait for timer to arm (switch goes high); show 8888 on display
		while(1){

			pressed = b.sample();
			
			display_times(8888);
			
			if (pressed & START_BUTTON){			//When the button is pressed, break out of the loop and start the timer
				break;
			}
		}
		
		//Wait for timer to start (switch goes low); show 0000 on display
		while(1){

			pressed = b.sample();
			
			display_times(0);
			
			if (!(pressed & START_BUTTON)){			//When the button is pressed, break out of the loop, read analog values, and start the timer
				analog_read_a(starting_sensor_values);
				analog_read_a(starting_sensor_values);

				break;
			}
		}
		
		//Start the timer
		timer_init();
		time = 0;
		
		//Wait for the races to finish, the stop button to be pressed, or the time to run out (9999 ms).
		while(1){
			display_times(time);

			b.sample();
			pressed = b.pressed();
			analog_read_a(sensor_values);
			
			for(uint8_t i = 0; i < LANE_COUNT; i++){
				if ((finish_times[i] == 0) && (starting_sensor_values[i] + ANALOG_THRESHOLD < sensor_values[i])){	//The sensors go high when the light beam is interrupted
					finish_times[i] = time;
				}
			}
			
			if (pressed & STOP_BUTTON || (finish_times[0] && finish_times[1] && finish_times[2]) || time >= 9999){
				for(uint8_t i = 0; i < LANE_COUNT; i++){
					//Stop any remaining timers
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
