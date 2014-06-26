#include "gait.h"

#define STEP_COUNT		22

static Point points[STEP_COUNT] = {
	Point(0,0,0),
	Point(-5,0,0),
	Point(-10,0,0),
	Point(-15,0,0),
	Point(-20,0,0),
	Point(-25,0,0),
	
	Point(-25,0,5),
	Point(-18,0,10),
	Point(-10,0,15),
	Point(-2,0,20),
	Point(6,0,20),
	Point(0,0,20),
	Point(8,0,20),
	Point(15,0,20),
	Point(20,0,15),
	Point(24,0,10),
	Point(25,0,5),
	
	Point(25,0,0),
	Point(20,0,0),
	Point(15,0,0),
	Point(10,0,0),
	Point(5,0,0),
};

Point gait_step(uint8_t leg_index, uint8_t step_index, double velocity){
	if (leg_index == FRONT_RIGHT || leg_index == MIDDLE_LEFT || leg_index == REAR_RIGHT){
		step_index += (STEP_COUNT / 2);
	}
	if (velocity < 0.3){
		return Point(0,0,0);
	}
	else {
		Point result = points[step_index % STEP_COUNT];
		result.x *= velocity;
		result.y *= velocity;
		return result;
	}
}

uint8_t gait_step_count(){
	return STEP_COUNT;
}
