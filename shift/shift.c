#include "shift.h"

static volatile uint8_t *_data_port = 0;
static volatile uint8_t *_clock_port = 0;
static volatile uint8_t *_latch_port = 0;

static uint8_t _data_pin = 0;
static uint8_t _clock_pin = 0;
static uint8_t _latch_pin = 0;

void shift_init(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin){
	_data_port = data_port;
	_clock_port = clock_port;
	_latch_port = latch_port;

	*(_data_port - 0x1) |= _BV(data_pin);
	*(_clock_port - 0x1) |= _BV(clock_pin);
	*(_latch_port - 0x1) |= _BV(latch_pin);
	
	_data_pin = data_pin;
	_clock_pin = clock_pin;
	_latch_pin = latch_pin;
}

uint8_t shift_out(uint8_t data){
	if (_data_port == 0) return -1;

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

	//Pulse latch to transfer contents to output
	*_latch_port &= ~_BV(_latch_pin);	
	*_latch_port |= _BV(_latch_pin);

	return 0;
}
