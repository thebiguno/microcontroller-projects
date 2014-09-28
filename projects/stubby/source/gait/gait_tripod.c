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
	Point(-30,0,0),
	
	Point(-25,0,10),
	Point(-15,0,20),
	Point(-5,0,30),
	Point(5,0,30),
	Point(10,0,30),
	Point(15,0,30),
	Point(20,0,30),
	Point(24,0,20),
	Point(25,0,10),
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
	Point(0,7,0),
	Point(-1,14,0),
	Point(-1,21,0),
	Point(-2,28,0),
	Point(-2,35,0),
	
	Point(-2,35,5),
	Point(-2,28,10),
	Point(-1,21,20),
	Point(-1,14,30),
	Point(0,7,30),
	Point(0,0,30),
	Point(0,-7,30),
	Point(-1,-14,30),
	Point(-1,-21,20),
	Point(-2,-28,10),
	Point(-2,-35,5),
	
	Point(-2,-35,0),
	Point(-2,-28,0),
	Point(-1,-21,0),
	Point(-1,-14,0),
	Point(0,-7,0),
};

Point gait_step(Leg leg, uint8_t step_index, double linear_velocity, double linear_angle, double rotational_velocity){
	if (leg.getIndex() == FRONT_RIGHT || leg.getIndex() == MIDDLE_LEFT || leg.getIndex() == REAR_RIGHT){
		step_index = (step_index + (STEP_COUNT / 2)) % STEP_COUNT;
	}

	Point result(0,0,0);
	
	Point linear_result(0,0,0);
	linear_result.add(linear_step_points[step_index]);
	linear_result.x *= linear_velocity;
	linear_result.y *= linear_velocity;
	
	linear_result.rotateXY(linear_angle);
	
	result.add(linear_result);
	
	Point rotational_result(0,0,0);
	rotational_result.add(rotational_step_points[step_index]);
	rotational_result.x *= fabs(rotational_velocity);
	rotational_result.y *= -1 * rotational_velocity;
	rotational_result.rotateXY(leg.getMountingAngle());
		
	result.add(rotational_result);
	
	return result;
}

uint8_t gait_step_count(){
	return STEP_COUNT;
}
