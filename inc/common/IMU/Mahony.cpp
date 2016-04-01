/**********************************************************************************************
 * Madwick's implementation of Mahoney's IMU.	Copied from 
 * http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
 * and converted to a C++ class by Wyatt Olson
 * 
 * Some helper methods copied from BitCraze's github repo 
 * https://github.com/bitcraze/crazyflie-firmware/blob/master/modules/src/sensfusion6.c
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include "Mahony.h"

using namespace digitalcave;

Mahony::Mahony(float kp, float ki, uint32_t time) :
	IMU(time),
	kp(kp),
	ki(ki),
	integralFBx(0),
	integralFBy(0),
	integralFBz(0)
{
}

void Mahony::compute(vector_t accel, vector_t gyro, vector_t mag, uint8_t armed, uint32_t time){
	float recipNorm;
    float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx, halfwy, halfwz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mag.x == 0.0f) && (mag.y == 0.0f) && (mag.z == 0.0f)) {
		compute(accel, gyro, armed, time);
		return;
	}

	float dt = (time - lastTime) / 1000.0;
	lastTime = time;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((accel.x == 0.0f) && (accel.y == 0.0f) && (accel.z == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
		accel.x *= recipNorm;
		accel.y *= recipNorm;
		accel.z *= recipNorm;

		// Normalise magnetometer measurement
		recipNorm = invSqrt(mag.x * mag.x + mag.y * mag.y + mag.z * mag.z);
		mag.x *= recipNorm;
		mag.y *= recipNorm;
		mag.z *= recipNorm;   

        // Auxiliary variables to avoid repeated arithmetic
        q0q0 = q0 * q0;
        q0q1 = q0 * q1;
        q0q2 = q0 * q2;
        q0q3 = q0 * q3;
        q1q1 = q1 * q1;
        q1q2 = q1 * q2;
        q1q3 = q1 * q3;
        q2q2 = q2 * q2;
        q2q3 = q2 * q3;
        q3q3 = q3 * q3;   

        // Reference direction of Earth's magnetic field
        hx = 2.0f * (mag.x * (0.5f - q2q2 - q3q3) + mag.y * (q1q2 - q0q3) + mag.z * (q1q3 + q0q2));
        hy = 2.0f * (mag.x * (q1q2 + q0q3) + mag.y * (0.5f - q1q1 - q3q3) + mag.z * (q2q3 - q0q1));
        bx = sqrt(hx * hx + hy * hy);
        bz = 2.0f * (mag.x * (q1q3 - q0q2) + mag.y * (q2q3 + q0q1) + mag.z * (0.5f - q1q1 - q2q2));

		// Estimated direction of gravity and magnetic field
		halfvx = q1q3 - q0q2;
		halfvy = q0q1 + q2q3;
		halfvz = q0q0 - 0.5f + q3q3;
        halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
        halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
        halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);  
	
		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex = (accel.y * halfvz - accel.z * halfvy) + (mag.y * halfwz - mag.z * halfwy);
		halfey = (accel.z * halfvx - accel.x * halfvz) + (mag.z * halfwx - mag.x * halfwz);
		halfez = (accel.x * halfvy - accel.y * halfvx) + (mag.x * halfwy - mag.y * halfwx);

		// Compute and apply integral feedback if enabled
		if(ki > 0.0f) {
			integralFBx += ki * halfex * dt;	// integral error scaled by Ki
			integralFBy += ki * halfey * dt;
			integralFBz += ki * halfez * dt;
			gyro.x += integralFBx;	// apply integral feedback
			gyro.y += integralFBy;
			gyro.z += integralFBz;
		}
		else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gyro.x += kp * halfex;
		gyro.y += kp * halfey;
		gyro.z += kp * halfez;
	}
	
	// Integrate rate of change of quaternion
	gyro.x *= (0.5f * dt);		// pre-multiply common factors
	gyro.y *= (0.5f * dt);
	gyro.z *= (0.5f * dt);
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gyro.x - qc * gyro.y - q3 * gyro.z);
	q1 += (qa * gyro.x + qc * gyro.z - q3 * gyro.y);
	q2 += (qa * gyro.y - qb * gyro.z + q3 * gyro.x);
	q3 += (qa * gyro.z + qb * gyro.y - qc * gyro.x); 
	
	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

void Mahony::compute(vector_t accel, vector_t gyro, uint8_t armed, uint32_t time){
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	float dt = (time - lastTime) / 1000.0;
	lastTime = time;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((accel.x == 0.0f) && (accel.y == 0.0f) && (accel.z == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
		accel.x *= recipNorm;
		accel.y *= recipNorm;
		accel.z *= recipNorm;        

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;
	
		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (accel.y * halfvz - accel.z * halfvy);
		halfey = (accel.z * halfvx - accel.x * halfvz);
		halfez = (accel.x * halfvy - accel.y * halfvx);

		// Compute and apply integral feedback if enabled
		if(ki > 0.0f) {
			integralFBx += ki * halfex * dt;	// integral error scaled by Ki
			integralFBy += ki * halfey * dt;
			integralFBz += ki * halfez * dt;
			gyro.x += integralFBx;	// apply integral feedback
			gyro.y += integralFBy;
			gyro.z += integralFBz;
		}
		else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gyro.x += kp * halfex;
		gyro.y += kp * halfey;
		gyro.z += kp * halfez;
	}
	
	// Integrate rate of change of quaternion
	gyro.x *= (0.5f * dt);		// pre-multiply common factors
	gyro.y *= (0.5f * dt);
	gyro.z *= (0.5f * dt);
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gyro.x - qc * gyro.y - q3 * gyro.z);
	q1 += (qa * gyro.x + qc * gyro.z - q3 * gyro.y);
	q2 += (qa * gyro.y - qb * gyro.z + q3 * gyro.x);
	q3 += (qa * gyro.z + qb * gyro.y - qc * gyro.x); 
	
	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}