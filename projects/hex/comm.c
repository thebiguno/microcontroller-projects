#include "comm.h"

static double _velocity; 	//0 is stopped, 1 is full speed ahead
static double _direction;	//0 is straight ahead, -1 is full left, 1 is full right

void comm_init(){
	serial_init_b(38400);
}

void comm_read(double *velocity, double *direction){
	while (serial_available()){
		uint8_t c;
		serial_read_c((char*) &c);

/*
		//Button pressed
		if ((c & 0xF0) == 0xF0){
			if ((c & 0x0F) == 0x04){
				_velocity = 1;
			}
			else if ((c & 0x0F) == 0x05){
				_direction = 1;
			}
			else if ((c & 0x0F) == 0x06){
				_direction = -1;
			}
		}
		else if (c == 0x80){
			_velocity = 0;
			_direction = 0;
		}
*/		
		if ((c & 0xE0) == 0x00){		//Left stick X
//			uint8_t value = c & 0x1F;
		}
		else if ((c & 0xE0) == 0x20){	//Left stick Y
			uint8_t value = c & 0x1F;
			if (value > 0x0A) {
				_velocity = 0x00;
			}
			else {
				_velocity = -1.0/15 * value + 1;
			}
		}
	}
	
	*velocity = _velocity;
	*direction = _direction;
	
	//Every once in a while, send the stick configuration again.  This should probably be 
	// done at least once every 5 seconds or so; by doing this every 256 calls to comm_read
	// we should be well ahead of that.
	static uint8_t count = 0;
	count++;
	if (count == 0){
		serial_write_b(0x41);	//Enable analog sticks
		serial_write_b(0x80);	//Disable button polling
	}
}
