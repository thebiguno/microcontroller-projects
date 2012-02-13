#include "matrix.h"

#include <avr/sfr_defs.h>

static volatile uint8_t *_data_port = 0;
static volatile uint8_t *_clock_port = 0;
static volatile uint8_t *_latch_port = 0;

static uint8_t _data_pin = 0;
static uint8_t _clock_pin = 0;
static uint8_t _latch_pin = 0;

static void matrix_data(uint8_t data){
	for (int i = 0; i < 8; i++){
		//Clear the pin first...
		*_data_port &= ~_BV(_data_pin);
		//... then set the bit (if appropriate).  We could probably
		// do this in one step, but this is more clear, plus speed is 
		// (probably) not critical here.
		*_data_port |= (((data >> (7 - i)) & 0x1) << _data_pin);
		
		//Pulse clock to shift in
		*_clock_port &= ~_BV(_clock_pin);
		*_clock_port |= _BV(_clock_pin);
	}
}

static void matrix_latch() {
	*_latch_port &= ~_BV(_latch_pin);	
	*_latch_port |= _BV(_latch_pin);
}

void matrix_init(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin){
	_data_port = data_port;
	_clock_port = clock_port;
	_latch_port = latch_port;

	// set ddr output
	*(_data_port - 0x1) |= _BV(data_pin);
	*(_clock_port - 0x1) |= _BV(clock_pin);
	*(_latch_port - 0x1) |= _BV(latch_pin);
	
	_data_pin = data_pin;
	_clock_pin = clock_pin;
	_latch_pin = latch_pin;
}
		
		
void matrix_draw(uint8_t red[], uint8_t green[]) {
	for (int i = 0; i < 8; i++) {
		matrix_data(0xFF-_BV(i));
		matrix_data(red[i]);
		matrix_data(green[i]);
		matrix_latch();
	}
	matrix_data(0xFF);
	matrix_data(0x00);
	matrix_data(0x00);
	matrix_latch();
}
