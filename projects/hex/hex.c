#include "hex.h"

leg_t* legs[6];

int main (void){
	leg_init(6);

	while(1){
		for (uint8_t i = 0; i < LEG_COUNT; i++){
			leg_set_position(i, 0, -1, -1, 500);
			leg_delay(1000);
			leg_set_position(i, 0, 0, 0, 500);
			leg_delay(1000);
		}
	}
}
