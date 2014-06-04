#include "Leg.h"

using namespace digitalcave;

/*
 * Abstraction of matrix rotation code.  Rotates a point at c1,c2 around an origin of o1,o2 by d degrees.
 */
void rotate2d(double *c1, double *c2, double o1, double o2, double angle){
	//Translate c1, c2 by o1, o2
	*c1 -= o1;
	*c2 -= o2;
	
	//Use rotational matrix to rotate point c1,c2 around the origin, resulting in new point n1,n2
	int16_t n1 = *c1 * cos(angle * -1) - *c2 * sin(angle * -1);
	int16_t n2 = *c1 * sin(angle * -1) + *c2 * cos(angle * -1);

	//Translate back to o1, o2
	*c1 = n1 + o1;
	*c2 = n2 + o2;
}

/*
 * Returns the angle which the servo needs to move from neutral.
 */
double solveServoTrapezoid(double length_a, double length_b, double length_c, double length_d, double angle_E, double angle_N){
	//See diagrams in doc/drive_system.png for description of sides and angles
	//Use law of cosines to find the length of the line between the control rod connection point and the servo shaft
	double length_e = sqrt(length_d * length_d + length_a * length_a - 2 * length_d * length_a * cos(angle_E));
	
	//Use law of cosines to find the angle between the line we just calculated and the line between the tibia joint and the servo shaft (D)
	double angle_D = acos((length_e * length_e + length_a * length_a - length_d * length_d) / (2 * length_e * length_a));
	//Use law of cosines to find the angle between servo_to_control_rod_length imaginary line and the servo horn
	double angle_C = acos((length_e * length_e + length_b * length_b - length_c * length_c) / (2 * length_e * length_b));

	#ifdef DEBUG_SIMULATION
	printf("length e: %3.1f; angle C + D: %2.3f; ", length_e, angle_D + angle_C);
	#endif
	
	return angle_N - (angle_C + angle_D);
}

Leg::Leg(uint8_t index, volatile uint8_t *tibia_port, uint8_t tibia_pin, volatile uint8_t *femur_port, uint8_t femur_pin, volatile uint8_t *coxa_port, uint8_t coxa_pin, double mounting_angle){
	this->index = index;
	this->port[TIBIA] = tibia_port;
	this->port[FEMUR] = femur_port;
	this->port[COXA] = coxa_port;
	this->pin[TIBIA] = tibia_pin;
	this->pin[FEMUR] = femur_pin;
	this->pin[COXA] = coxa_pin;
	this->mounting_angle = mounting_angle;
}

void Leg::setPosition(int16_t x, int16_t y, int16_t z){
	this->x = x;
	this->y = y;
	this->z = z;
	#ifdef DEBUG_SIMULATION
	printf("Desired position: %d, %d, %d\n", x, y, z);
	#endif

	double x1 = x;
	double y1 = y;
	
	//Rotate leg around 0, 0 such that the leg is pointing straight out at angle 0 (straight right).
	rotate2d(&x1, &y1, 0, 0, this->mounting_angle);
	//Translate the leg according to the leg offset, to put the coxa joint at co-ordinates 0,0.
	x1 -= LEG_OFFSET;
	
	//Find the angle of the leg, used to set the coxa joint.  See figure 2.1, 'coxa angle'.
	double coxa_angle = atan2(y1, x1);
	
	//Find the length of the leg, from coxa joint to end of tibia, on the x,y plane (to be later 
	// used for X,Z inverse kinematics).  See figure 2.1, 'leg length', 'x1', and 'y1'.
	double leg_length = sqrt((x1 * x1) + (y1 * y1));

	//Find the distance between the femur joint and the end of the tibia.  Do this using the
	// right triangle of (FEMUR_HEIGHT + COXA_HEIGHT - z), (leg_length - COXA_LENGTH).  See figure 
	// 2.2, 'leg extension'
	double leg_extension = sqrt((FEMUR_HEIGHT + COXA_HEIGHT - z) * (FEMUR_HEIGHT + COXA_HEIGHT - z) + (leg_length - COXA_LENGTH) * (leg_length - COXA_LENGTH));
	
	//Now that we have all three sides of the triangle, we can use law of cosines to find angles 
	// of the tibia.  See figure 2.3 for a diagram of this.
	double tibia_angle = acos(((FEMUR_LENGTH * FEMUR_LENGTH) + (TIBIA_LENGTH * TIBIA_LENGTH) - (leg_extension * leg_extension)) / (2 * FEMUR_LENGTH * TIBIA_LENGTH));
	double femur_angle = acos(((FEMUR_LENGTH * FEMUR_LENGTH) + (leg_extension * leg_extension) - (TIBIA_LENGTH * TIBIA_LENGTH)) / (2 * FEMUR_LENGTH * leg_extension));
	
	#ifdef DEBUG_SIMULATION
	printf(" IK: x,y,z: %d,%d,%d; x1,y1: %3.1f,%3.1f, leg_length: %3.1f; leg_extension: %3.1f\n", x,y,z, x1,y1, leg_length, leg_extension);
	#endif
	
	this->setTibiaAngle(tibia_angle);
	this->setFemurAngle(femur_angle);
	this->setCoxaAngle(coxa_angle);
}

void Leg::getPosition(int16_t *x, int16_t *y, int16_t *z){
	*x = this->x;
	*y = this->y;
	*z = this->z;
}

volatile uint8_t* Leg::getPort(uint8_t joint){
	return this->port[joint];
}

uint8_t Leg::getPin(uint8_t joint){
	return this->pin[joint];
}

int8_t Leg::getOffset(uint8_t joint){
	return this->offset[joint];
}
			
void Leg::setOffset(uint8_t joint, int8_t offset){
	this->offset[joint] = offset;
}

void Leg::setTibiaAngle(double desired_angle){
	#ifdef DEBUG_SIMULATION
	printf(" Tibia: desired angle: %2.3f; ", desired_angle);
	#endif

	//See diagrams in doc/drive_system.png for description of sides and angles
	double angle_S = solveServoTrapezoid(TIBIA_A, TIBIA_B, TIBIA_C, TIBIA_D, desired_angle + TIBIA_E_OFFSET_ANGLE, TIBIA_NEUTRAL_SERVO_ANGLE);
	
	#ifdef DEBUG_SIMULATION
	printf(" servo angle: %2.3f; ", angle_S);
	#endif

	pwm_set_phase_batch((this->index * JOINT_COUNT) + TIBIA, (uint16_t) NEUTRAL + angle_S * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL));
}

void Leg::setFemurAngle(double desired_angle){
	#ifdef DEBUG_SIMULATION
	printf(" Femur: desired angle: %2.3f; ", desired_angle);
	#endif

	//See diagrams in doc/drive_system.png for description of sides and angles
	double angle_S = solveServoTrapezoid(FEMUR_A, FEMUR_B, FEMUR_C, FEMUR_D, desired_angle + FEMUR_E_OFFSET_ANGLE, FEMUR_NEUTRAL_SERVO_ANGLE);
	
	#ifdef DEBUG_SIMULATION
	printf(" servo angle: %2.3f; ", angle_S);
	#endif

	pwm_set_phase_batch((this->index * JOINT_COUNT) + FEMUR, (uint16_t) NEUTRAL + angle_S * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL));
}

void Leg::setCoxaAngle(double coxa_angle){
	#ifdef DEBUG_SIMULATION
	printf(" desired coxa angle: %2.3f\n", coxa_angle);
	#endif
	
	//This hopefully is close enough, so no need to bother with the trapezoid calculations...
	pwm_set_phase_batch((this->index * JOINT_COUNT) + COXA, NEUTRAL + (((coxa_angle + (5 * M_PI / 180)) * 1.75 /*+ this->offset[COXA]*/) * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL)));
}

