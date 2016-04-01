#include "IMU.h"
using namespace digitalcave;

IMU::IMU(uint32_t time) :
	lastTime(time),
	q0(1),
	q1(0),
	q2(0),
	q3(0)
{
}

vector_t IMU::getEuler(){
	vector_t result;
	result.x = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2);
	result.y = asin(-2 * (q1*q3 - q0*q2));
	result.z = atan2f(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3);
	return result;
}

float IMU::getZAcceleration(vector_t accel){
	float gx, gy, gz; // estimated gravity direction

	gx = 2 * (q1*q3 - q0*q2);
	gy = 2 * (q0*q1 + q2*q3);
	gz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	// return vertical acceleration without gravity
	// (A dot G) / |G| - 1G (|G| = 1) -> (A dot G)
	return ((accel.x*gx + accel.y*gy + accel.z*gz));
}