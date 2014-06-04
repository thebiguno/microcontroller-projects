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
	
	double x = -64;
	double y = 114;
	double z = 0;
	for (uint8_t i = 0; i < 64; i++){
		legs[FRONT_LEFT].setPosition(x, y, z);
		//x = x-0.25;
		y = y-0.8;
		//z = z+;
	}
	
	return 0;
}
