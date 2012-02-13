#include "matrix.h"

#include <avr/sfr_defs.h>

static volatile uint8_t *_data_port = 0;
static volatile uint8_t *_clock_port = 0;
static volatile uint8_t *_latch_port = 0;

static uint8_t _data_pin = 0;
static uint8_t _clock_pin = 0;
static uint8_t _latch_pin = 0;

static void matrix_data(uint8_t data){
	for (uint8_t i = 0; i < 8; i++){
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

uint8_t bits_set(uint8_t v) {
	uint8_t c = v - ((v >> 1) & 0x55);
	c = ((c >> 0x02) & 0x33) + (c & 0x33);
	c = ((c >> 0x04) + c) & 0x0F;
	return c;
}

void matrix_draw(uint8_t red[], uint8_t green[]) {
	static uint8_t color = 0;
	static uint8_t row = 0;
	static uint8_t dc_r[8];
	static uint8_t dc_g[8];
	
	shift_data(0xFF-_BV(row));
	if (color == 0) {
		if (bits_set(red[row]) >= dc_r[row]) {
			shift_data(red[row]);
		} else {
			shift_data(0x00);
		}
		shift_data(0x00);
		dc_r[row]++;
		if (dc_r[row] > 7) dc_r[row] = 0;
	} else {
		shift_data(0x00);
		if (bits_set(green[row]) >= dc_g[row]) {
			shift_data(green[row]);
		} else {
			shift_data(0x00);
		}
		dc_g[row]++;
		if (dc_g[row] > 7) dc_g[row] = 0;

		row++;
		if (row > 7) row = 0;
	}
	
	color = ~color;
}
