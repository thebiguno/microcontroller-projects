#include "comm.h"

static double _velocity; 	//0 is stopped, 1 is full speed ahead
static double _direction;	//0 is straight ahead, -1 is full left, 1 is full right

void comm_init(){
	serial_init_b(38400);
}

void comm_read(double *velocity, double *direction){
	while (serial_available()){
		uint8_t b;
		serial_read_b(&b);

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
		if ((b & 0xE0) == 0x00){		//Left stick X
			uint8_t value = b & 0x1F;
			if (value > 0x08 && value < 0x17) {
				//Dead zone from 8 to 23 (i.e. 8 on either side of center).  This may be excessive; adjust as needed.
				_direction = 0x00;
			}
			else {
				//The PSX controller sends X=0 for all the way left; we want to translate this to -1 for left.
				_direction = 1.0/15 * value - 1;
			}
		}
		else if ((b & 0xE0) == 0x20){	//Left stick Y
			uint8_t value = b & 0x1F;
			if (value > 0x0A && value < 0x15) {
				//Dead zone from 10 to 21 (i.e. 6 on either side of center).  This may be excessive; adjust as needed.
				_velocity = 0x00;
			}
			else {
				//The PSX controller sends X=0 for all the way forward; we want to translate this to 1 for forward.
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
