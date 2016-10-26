#include "gait.h"

#include <math.h>
#include <dcutil/dcmath.h>

#include "../types/Point.h"

#define STEP_COUNT		22

using namespace digitalcave;

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

	Point(-25,0,20),
	Point(-15,0,40),
	Point(-5,0,50),
	Point(5,0,50),
	Point(10,0,50),
	Point(15,0,50),
	Point(20,0,50),
	Point(24,0,40),
	Point(25,0,20),
	Point(25,0,10),

	Point(25,0,0),
	Point(20,0,0),
	Point(15,0,0),
	Point(10,0,0),
	Point(5,0,0),
};

//The lookup table for moving a foot such that the robot
// rotates clockwise on the spot.  Starting at neutral,
// the foot rotates counter clockwise, then lifts, and
// moves clockwise.  We do not include any Z component,
// since the linear_step_points have it, and they are
// always included.
static Point rotational_step_points[STEP_COUNT] = {
	Point(0,0,0),
	Point(0,4,0),
	Point(-1,8,0),
	Point(-1,12,0),
	Point(-2,16,0),
	Point(-2,20,0),
	Point(-3,24,0),

	Point(-2,24,0),
	Point(-1,16,0),
	Point(-1,8,0),
	Point(0,0,0),
	Point(0,-8,0),
	Point(0,-14,0),
	Point(-1,-18,0),
	Point(-1,-22,0),
	Point(-2,-24,0),

	Point(-2,-24,0),
	Point(-2,-20,0),
	Point(-2,-16,0),
	Point(-1,-12,0),
	Point(-1,-8,0),
	Point(0,-4,0),
};

void gait_step(Stubby* stubby){
	static uint8_t step_index = 0;
	static uint32_t last_time = 0;

	uint32_t time = timer_millis();

	if (time - last_time > GAIT_STEP_INTERVAL){
		if (stubby->getRotationalVelocity() >= 0.3 || stubby->getRotationalVelocity() <= -0.3 || stubby->getLinearVelocity() >= 0.3){
			Leg** legs = stubby->getLegs();
			for(uint8_t i = 0; i < LEG_COUNT; i++){
				Leg* leg = legs[i];

				//Three of the legs are out of phase with the other three (otherwise it is pretty hard to actually move...)
				uint8_t j = step_index;
				if (leg->getIndex() == FRONT_RIGHT || leg->getIndex() == MIDDLE_LEFT || leg->getIndex() == REAR_RIGHT){
					i = (step_index + (STEP_COUNT / 2)) % STEP_COUNT;
				}

				Point result(0,0,0);

				Point linear_result(0,0,0);
				linear_result.add(linear_step_points[j]);
				linear_result.x *= stubby->getLinearVelocity();
				linear_result.y *= stubby->getLinearVelocity();

				linear_result.rotateXY(stubby->getLinearAngle());

				result.add(linear_result);

				Point rotational_result(0,0,0);
				rotational_result.add(rotational_step_points[j]);
				rotational_result.x *= fabs(stubby->getRotationalVelocity());
				rotational_result.y *= stubby->getRotationalVelocity();
				rotational_result.rotateXY(leg->getMountingAngle());

				result.add(rotational_result);

				step_index++;
				if (step_index >= STEP_COUNT){
					step_index = 0;
				}

				legs[i]->setOffset(result);
			}
		}
		else {
			Point result(0,0,0);
			for(uint8_t i = 0; i < LEG_COUNT; i++){
				stubby->getLegs()[i]->setOffset(result);
			}
			step_index = 0;
		}

		pwm_apply_batch();
		last_time = time;
	}
}
