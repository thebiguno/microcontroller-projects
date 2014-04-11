#include "comm.h"

static double _velocity; 	//0 is stopped, 1 is full speed ahead
static double _direction;	//0 is straight ahead, -1 is full left, 1 is full right
static uint8_t _special;		//Special behaviours; set bits for each

void comm_init(){
	serial_init_b(38400);
}

void comm_read(double *velocity, double *direction, uint8_t *special){
	static uint16_t button_press_mask = 0x00;
	
	while (serial_available()){
		uint8_t b;
		serial_read_b(&b);

		//Button pressed
		if ((b & 0xF0) == 0xF0){
			button_press_mask |= _BV(b & 0x0F);
		}
		else if ((b & 0xF0) == 0xE0){
			button_press_mask &= ~_BV(b & 0x0F);
		}
		else if (b == 0x80){
			button_press_mask = 0x00;
		}
		
		if (button_press_mask & PSB_CROSS){
			_special |= SPECIAL_RESET;
		}

		if (button_press_mask & PSB_PAD_UP){
			_velocity = 1;
		}
		else if (button_press_mask & PSB_PAD_DOWN){
			_velocity = -1;
		}
		else {
			//_velocity = 0;
		}
		
		if (button_press_mask & PSB_R2){
			_direction = 1;
		}
		else if (button_press_mask & PSB_L2){
			_direction = -1;
		}
		else {
			//_direction = 0;
		}
		
		if (button_press_mask == 0x00){
			_velocity = 0;
			_direction = 0;
			_special = 0;
		}

		
/*
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
*/
	}
	
	*velocity = _velocity;
	*direction = _direction;
	*special = _special;
}
