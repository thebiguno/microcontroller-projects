#include "../source/Stubby.h"
#include "../source/Leg.h"

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	volatile uint8_t p = 0;
	Leg legs[LEG_COUNT] = {
		Leg(FRONT_LEFT,		&p, p, &p, p, &p, p, 2 * LEG_MOUNTING_ANGLE, Point(-65, 113, 0)),
		Leg(MIDDLE_LEFT,	&p, p, &p, p, &p, p, 3 * LEG_MOUNTING_ANGLE, Point(-130, 0, 0)),
		Leg(REAR_LEFT,		&p, p, &p, p, &p, p, 4 * LEG_MOUNTING_ANGLE, Point(-65, -113, 0)),
		Leg(REAR_RIGHT,		&p, p, &p, p, &p, p, 5 * LEG_MOUNTING_ANGLE, Point(65, -113, 0)),
		Leg(MIDDLE_RIGHT,	&p, p, &p, p, &p, p, 0 * LEG_MOUNTING_ANGLE, Point(130, 0, 0)),
		Leg(FRONT_RIGHT,	&p, p, &p, p, &p, p, 1 * LEG_MOUNTING_ANGLE, Point(65, 113, 0))
	};
	
	//for (uint8_t l = 0; l < LEG_COUNT; l++){
	//	legs[l].resetPosition();
	//}
	
	legs[MIDDLE_RIGHT].resetPosition();
	legs[MIDDLE_RIGHT].setOffset(Point(0,0,-10));
	legs[MIDDLE_RIGHT].setOffset(Point(0,0,15));
	
	Point point(1, 2, 3);
	point.rotateXYZ(Point(1,0,0), 90 * M_PI / 180);
	
	//printf("\nX,Y,Z: %d,%d,%d\n", point.x, point.y, point.z);
	
	return 0;
}
