#include "../source/Stubby.h"
#include "../source/Leg.h"

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	volatile uint8_t p = 0;
	Leg legs[LEG_COUNT] = {
		Leg(FRONT_LEFT,		&p, p, &p, p, &p, p, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0)),
		Leg(MIDDLE_LEFT,	&p, p, &p, p, &p, p, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0)),
		Leg(REAR_LEFT,		&p, p, &p, p, &p, p, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0)),
		Leg(REAR_RIGHT,		&p, p, &p, p, &p, p, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0)),
		Leg(MIDDLE_RIGHT,	&p, p, &p, p, &p, p, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0)),
		Leg(FRONT_RIGHT,	&p, p, &p, p, &p, p, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0))
	};
	
	//for (uint8_t l = 0; l < LEG_COUNT; l++){
	//	legs[l].resetPosition();
	//}
	
	//legs[MIDDLE_RIGHT].resetPosition();
	legs[MIDDLE_RIGHT].setOffset(Point(0,-30,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,-20,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,-10,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,0,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,10,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,20,0));
	legs[MIDDLE_RIGHT].setOffset(Point(0,30,0));

//	for (uint8_t l = 0; l < LEG_COUNT; l++){
//		legs[l].resetPosition();
//	}
	
//	for (uint8_t j = 0; j < JOINT_COUNT; j++){
//		pwm_set_phase_batch(MIDDLE_RIGHT * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[MIDDLE_RIGHT].getCalibration(j) * 10));
//	}

	
	return 0;
}
