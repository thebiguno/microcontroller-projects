/**********************************************************************************************
 * Madgwick's IMU implementation.	Copied from BitCraze's github 
 * repo (https://github.com/bitcraze/crazyflie-firmware/blob/master/modules/src/sensfusion6.c)
 * and converted to a C++ class by Wyatt Olson
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include <stdio.h>

#include "Madgwick.h"

using namespace digitalcave;

Madgwick::Madgwick(double beta, uint32_t time) :
		beta(beta),
		lastTime(time),
		q0(1),
		q1(0),
		q2(0),
		q3(0)
{
}

void Madgwick::compute(vector_t accel, vector_t gyro, uint32_t time){
	double dt = (time - lastTime) / 1000.0;
	lastTime = time;

	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gyro.x - q2 * gyro.y - q3 * gyro.z);
	qDot2 = 0.5f * (q0 * gyro.x + q2 * gyro.z - q3 * gyro.y);
	qDot3 = 0.5f * (q0 * gyro.y - q1 * gyro.z + q3 * gyro.x);
	qDot4 = 0.5f * (q0 * gyro.z + q1 * gyro.y - q2 * gyro.x);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((accel.x == 0.0f) && (accel.y == 0.0f) && (accel.z == 0.0f))){
		// Normalise accelerometer measurement
		recipNorm = invSqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
		accel.x *= recipNorm;
		accel.y *= recipNorm;
		accel.z *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * accel.x + _4q0 * q1q1 - _2q1 * accel.y;
		s1 = _4q1 * q3q3 - _2q3 * accel.x + 4.0f * q0q0 * q1 - _2q0 * accel.y - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * accel.z;
		s2 = 4.0f * q0q0 * q2 + _2q0 * accel.x + _4q2 * q3q3 - _2q3 * accel.y - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * accel.z;
		s3 = 4.0f * q1q1 * q3 - _2q1 * accel.x + 4.0f * q2q2 * q3 - _2q2 * accel.y;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * dt;
	q1 += qDot2 * dt;
	q2 += qDot3 * dt;
	q3 += qDot4 * dt;

	// Normalise quaternion
	recipNorm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

vector_t Madgwick::getEuler(){
	double gx, gy, gz; // estimated gravity direction

	gx = 2 * (q1*q3 - q0*q2);
	gy = 2 * (q0*q1 + q2*q3);
	gz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	if (gx>1) gx=1;
	if (gx<-1) gx=-1;

	vector_t result;
	result.x = atan2(gy, gz);
	result.y = -1 * asin(gx);
	result.z = atan2(2*(q0*q3 + q1*q2), q0*q0 + q1*q1 - q2*q2 - q3*q3);
	return result;
}

double Madgwick::getBeta() {
	return beta;
}

void Madgwick::setBeta(double beta) {
	this->beta = beta;
}
