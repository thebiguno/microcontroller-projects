#include "gait.h"

#define STEP_COUNT		22

//The lookup table for moving a foot such that the robot
// moves in a straight direction.  The foot starts from
// neutral, and moves along the X axis in a negative
// direction, then lifts up and moves forward.
static Point linear_step_points[STEP_COUNT] = {
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

//The lookup table for moving a foot such that the robot
// rotates clockwise on the spot.  Starting at neutral,
// the foot rotates counter clockwise, then lifts, and
// moves clockwise.
static Point rotational_step_points[STEP_COUNT] = {
	Point(0,0,0),
	Point(0,8,0),
	Point(-1,13,0),
	Point(-1,18,0),
	Point(-2,22,0),
	Point(-3,27,0),
	
	Point(-3,27,5),
	Point(-2,22,10),
	Point(-1,18,15),
	Point(-1,13,20),
	Point(0,8,20),
	Point(0,0,20),
	Point(0,-8,20),
	Point(-1,-13,20),
	Point(-1,-18,15),
	Point(-2,-22,10),
	Point(-3,-27,5),
	
	Point(-3,-27,0),
	Point(-2,-22,0),
	Point(-1,-18,0),
	Point(-1,-13,0),
	Point(0,-8,0),
};

Point gait_step(uint8_t leg_index, uint8_t step_index, double linear_velocity, double linear_angle, double rotational_velocity){
	if (leg_index == FRONT_RIGHT || leg_index == MIDDLE_LEFT || leg_index == REAR_RIGHT){
		step_index += (STEP_COUNT / 2);
	}

	Point result(0,0,0);
	
	if (linear_velocity > 0.3){
		Point linear_result(0,0,0);
		linear_result.add(linear_step_points[step_index % STEP_COUNT]);
		linear_result.x *= linear_velocity;
		linear_result.y *= linear_velocity;
		
		linear_result.rotateXY(linear_angle);
		
		result.add(linear_result);
	}
	
	if (rotational_velocity > 0.3){
		Point rotational_result(0,0,0);
		rotational_result.add(rotational_step_points[step_index % STEP_COUNT]);
		rotational_result.x *= rotational_velocity;
		rotational_result.y *= rotational_velocity;
		result.add(rotational_result);
	}
	
	if (result.x > 30) result.x = 30;
	else if (result.x < -30) result.x = -30;

	if (result.y > 30) result.y = 30;
	else if (result.y < -30) result.y = -30;
	
	if (result.z > 20) result.z = 20;
	else if (result.z < -20) result.z = -20;

	return result;
}

uint8_t gait_step_count(){
	return STEP_COUNT;
}
