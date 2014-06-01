#include "Leg.h"

using namespace digitalcave;

Leg::Leg(uint8_t index, volatile uint8_t *tibia_port, uint8_t tibia_pin, volatile uint8_t *femur_port, uint8_t femur_pin, volatile uint8_t *coxa_port, uint8_t coxa_pin, double mounting_angle){
	this->index = index;
	this->port[TIBIA] = tibia_port;
	this->port[FEMUR] = femur_port;
	this->port[COXA] = coxa_port;
	this->pin[TIBIA] = tibia_pin;
	this->pin[TIBIA] = femur_pin;
	this->pin[TIBIA] = coxa_pin;
	this->mounting_angle = mounting_angle;
}

void Leg::rotate2d(int16_t *c1, int16_t *c2, int16_t o1, int16_t o2, double angle){
	//Translate c1, c2 by o1, o2
	*c1 -= o1;
	*c2 -= o2;
	
	//Use rotational matrix to rotate point c1,c2 around the origin, resulting in new point n1,n2
	int16_t n1 = *c1 * cos(angle) - *c2 * sin(angle);
	int16_t n2 = *c1 * sin(angle) + *c2 * cos(angle);

	//Translate back to o1, o2
	*c1 = n1 + o1;
	*c2 = n2 + o2;
}

void Leg::setPosition(int16_t x, int16_t y, int16_t z){
	int16_t x1 = x;
	int16_t y1 = y;
	
	//Rotate and translate leg to put 0,0 at coxa joint, pointing straight right.
	this->rotate2d(&x1, &y1, 0, 0, this->mounting_angle);
	x1 -= LEG_OFFSET;
	
	//Find the length of the leg, from coxa joint to end of tibia, on the x,y plane (to be later used for X,Z IK)
	double leg_length = sqrt(x1 * x1 + y1 * y1);

	//Find the angle of the leg, used to set the coxa joint
	double coxa_angle = atan2(y1, x1);
	
	//Find the distance between the femur joint and the end of the tibia.  Do this using the right triangle of
	// (COXA_HEIGHT - z), x (where x is relative to the femur joint, as it was translated earlier).
	uint16_t leg_extension = sqrt((COXA_HEIGHT - z) * (COXA_HEIGHT - z) + (x - leg_length) * (x - leg_length));
	
	//Now that we have all three sides of the triangle, we can use law of cosines to find angles.
	double femur_angle = acos((FEMUR_LENGTH * FEMUR_LENGTH + leg_extension * leg_extension - TIBIA_LENGTH * TIBIA_LENGTH) / (2 * FEMUR_LENGTH * leg_extension));
	double tibia_angle = acos((FEMUR_LENGTH * FEMUR_LENGTH + TIBIA_LENGTH * TIBIA_LENGTH - leg_extension * leg_extension) / (2 * FEMUR_LENGTH * TIBIA_LENGTH));
	
	this->setTibiaAngle(tibia_angle);
	this->setFemurAngle(femur_angle);
	this->setCoxaAngle(coxa_angle);
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

void Leg::setTibiaAngle(double tibia_angle){
	pwm_set_phase_batch((this->index * JOINT_COUNT) + TIBIA, NEUTRAL + ((tibia_angle + this->offset[TIBIA]) * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL)));
}

void Leg::setFemurAngle(double femur_angle){
	pwm_set_phase_batch((this->index * JOINT_COUNT) + FEMUR, NEUTRAL + ((femur_angle + this->offset[FEMUR]) * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL)));
}

void Leg::setCoxaAngle(double coxa_angle){
	pwm_set_phase_batch((this->index * JOINT_COUNT) + COXA, NEUTRAL + ((coxa_angle + this->offset[COXA]) * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL)));
}