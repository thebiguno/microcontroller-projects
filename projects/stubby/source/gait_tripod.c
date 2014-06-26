#include "gait.h"

#define STEP_COUNT		22

static Point points[STEP_COUNT] = {
	Point(0,0,0),
	Point(0,-3,0),
	Point(0,-6,0),
	Point(0,-9,0),
	Point(0,-12,0),
	Point(0,-15,0),
	
	Point(0,-15,3),
	Point(0,-12,6),
	Point(0,-10,10),
	Point(0,-6,15),
	Point(0,-2,15),
	Point(0,0,15),
	Point(0,2,15),
	Point(0,6,15),
	Point(0,10,10),
	Point(0,12,6),
	Point(0,15,3),
	
	Point(0,15,0),
	Point(0,12,0),
	Point(0,9,0),
	Point(0,6,0),
	Point(0,3,0),
};

Point gait_step(uint8_t leg_index, uint8_t step_index){
	if (leg_index == FRONT_RIGHT || leg_index == MIDDLE_LEFT || leg_index == REAR_RIGHT){
		step_index += (STEP_COUNT / 2);
	}
	return points[step_index % STEP_COUNT];
}

uint8_t gait_step_count(){
	return STEP_COUNT;
}
