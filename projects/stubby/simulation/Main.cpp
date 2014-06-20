#include "../source/Stubby.h"

using namespace digitalcave;

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	volatile uint8_t p = 0;
	Leg legs[LEG_COUNT] = {
		Leg(FRONT_LEFT,		&p, p, &p, p, &p, p, 2 * LEG_MOUNTING_ANGLE),
		Leg(FRONT_RIGHT,	&p, p, &p, p, &p, p, 1 * LEG_MOUNTING_ANGLE),
		Leg(MIDDLE_LEFT,	&p, p, &p, p, &p, p, 3 * LEG_MOUNTING_ANGLE),
		Leg(MIDDLE_RIGHT,	&p, p, &p, p, &p, p, 0 * LEG_MOUNTING_ANGLE),
		Leg(REAR_LEFT,		&p, p, &p, p, &p, p, 4 * LEG_MOUNTING_ANGLE),
		Leg(REAR_RIGHT,		&p, p, &p, p, &p, p, 5 * LEG_MOUNTING_ANGLE)
	};
	
	legs[FRONT_LEFT].setPosition(-65, 130, 0);
	legs[FRONT_LEFT].setPosition(-65, 120, 0);
	legs[FRONT_LEFT].setPosition(-65, 110, 0);
	legs[FRONT_LEFT].setPosition(-65, 100, 0);
	legs[FRONT_LEFT].setPosition(-65, 90, 0);
	legs[FRONT_LEFT].setPosition(-65, 80, 0);
	return 0;
}
