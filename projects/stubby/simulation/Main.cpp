#include "../source/Stubby.h"

using namespace digitalcave;

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	volatile uint8_t p = 0;
	Leg legs[LEG_COUNT] = {
		Leg(FRONT_LEFT,		&p, p, &p, p, &p, p, 2 * LEG_MOUNTING_ANGLE, Point(-65, 112, 0)),
		Leg(MIDDLE_LEFT,	&p, p, &p, p, &p, p, 3 * LEG_MOUNTING_ANGLE, Point(-130, 0, 0)),
		Leg(REAR_LEFT,		&p, p, &p, p, &p, p, 4 * LEG_MOUNTING_ANGLE, Point(-65, -112, 0)),
		Leg(REAR_RIGHT,		&p, p, &p, p, &p, p, 5 * LEG_MOUNTING_ANGLE, Point(65, -112, 0)),
		Leg(MIDDLE_RIGHT,	&p, p, &p, p, &p, p, 0 * LEG_MOUNTING_ANGLE, Point(130, 0, 0)),
		Leg(FRONT_RIGHT,	&p, p, &p, p, &p, p, 1 * LEG_MOUNTING_ANGLE, Point(65, 112, 0))
	};
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	return 0;
}
