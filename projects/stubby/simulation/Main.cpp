#include "../source/Stubby.h"

using namespace digitalcave;

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	volatile uint8_t p = 0;
	Leg legs[LEG_COUNT] = {
		Leg(FRONT_LEFT,		&p, p, &p, p, &p, p, 2 * M_PI / 3),
		Leg(FRONT_RIGHT,	&p, p, &p, p, &p, p, M_PI / 3),
		Leg(MIDDLE_LEFT,	&p, p, &p, p, &p, p, 3 * M_PI / 3),
		Leg(MIDDLE_RIGHT,	&p, p, &p, p, &p, p, 0),
		Leg(REAR_LEFT,		&p, p, &p, p, &p, p, 4 * M_PI / 3),
		Leg(REAR_RIGHT,		&p, p, &p, p, &p, p, 5 * M_PI / 3)
	};
	
	int16_t x = 120;
	int16_t y = 6;
	int16_t z = 0;
	for (uint8_t i = 0; i < 4; i++){
		legs[MIDDLE_RIGHT].setPosition(x, y, z);
		x = x-3;
		y = y-2;
		z = z+1;
	}
	
	return 0;
}
