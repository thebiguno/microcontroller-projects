#include "hex.h"

leg_t legs[LEG_COUNT];

double velocity; 	//0 is stopped, 1 is full speed ahead
double turn;		//0 is straight ahead, -1 is full left, 1 is full right

void hex_read_serial_command(){
	while (serial_available()){
		uint8_t c;
		serial_read_c((char*) &c);

		//Button pressed
		if ((c & 0xF0) == 0xF0){
			if ((c & 0x0F) == 0x04){
				velocity = 1;
			}
			else if ((c & 0x0F) == 0x05){
				turn = 1;
			}
			else if ((c & 0x0F) == 0x06){
				turn = -1;
			}
		}
		else if (c == 0x80){
			velocity = 0;
			turn = 0;
		}
/*		
		if ((c & 0xE0) == 0x00){		//Left stick X
//			uint8_t value = c & 0x1F;
		}
		else if ((c & 0xE0) == 0x20){	//Left stick Y
			uint8_t value = c & 0x1F;
			if (value > 0x0D) {
				velocity = 0x00;
			}
			else {
				velocity = -1.0/15 * value + 1;
			}
		}
*/
	}
}

double get_velocity(){
	return velocity;
}

double get_turn(){
	return turn;
}

int main (void){
	serial_init_b(38400);
	leg_init();
	
	DDRA |= _BV(PORTA1);
	
//	ripple_gait_init();
	
	while(1){
		if (velocity > 0.0001){
			ripple_step();
		}
		else {
			leg_delay_ms(5);
		}
//		serial_write_c(0x41);	//Enable analog sticks
//		serial_write_c(0x80);	//Disable button polling
	}
}
