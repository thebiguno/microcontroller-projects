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
	
	legs[FRONT_LEFT].setPosition(-64, 112, 0);
	legs[FRONT_RIGHT].setPosition(64, 112, 0);
	legs[MIDDLE_LEFT].setPosition(-120, 0, 0);
	legs[MIDDLE_RIGHT].setPosition(120, 0, 0);
	legs[REAR_LEFT].setPosition(-64, -112, 0);
	legs[REAR_RIGHT].setPosition(64, -112, 0);
	
	return 0;
}
