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
	
	for (int8_t y = 32; y > -32; y-=2){
		legs[MIDDLE_RIGHT].setPosition(140, y, 0);
	}
	
	return 0;
}
