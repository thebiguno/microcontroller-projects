/**********************************************************************************************
 * Basic PID class.
 * by Wyatt Olson <wyatt@digitalcave.ca>
 *
 * Based (very loosely) on the Arduino PID Library - Version 1.1.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include "PID.h"

using namespace digitalcave;

PID::PID(float kp, float ki, float kd, uint8_t direction, uint32_t time) :
	lastTime(time),
	integratedError(0),
	lastMeasured(0)
{
	setOutputLimits(0, 255);
	setDirection(direction);
	setTunings(kp, ki, kd);
}

float PID::compute(float setPoint, float measured, uint32_t time){
	uint32_t currentPeriod = time - lastTime;
	if (currentPeriod == 0) currentPeriod = 1;	//Calling this more frequently than once per ms is not recommended...
	float periodSec = currentPeriod / 1000.0;	//Period in seconds; used to normalize ki / kd values across different periods

	//Compute all the working error variables
	float error = setPoint - measured;
	integratedError += (ki * periodSec * error);

	if (integratedError > outMax) integratedError = outMax;
	else if (integratedError < outMin) integratedError = outMin;

	float derivative = (measured - lastMeasured);

	// Compute PID Output
	float result = kp * error + integratedError - kd / periodSec * derivative;

	if (result > outMax) result = outMax;
	else if(result < outMin) result = outMin;

	//Remember some variables for next time
	lastMeasured = measured;
	lastTime = time;

	return result;
}

void PID::getTunings(float* kp, float* ki, float* kd){
	if(direction == DIRECTION_REVERSE){
		*kp = (0 - this->kp);
		*ki = (0 - this->ki);
		*kd = (0 - this->kd);
	}
	else {
		*kp = this->kp;
		*ki = this->ki;
		*kd = this->kd;
	}
}

void PID::setTunings(float kp, float ki, float kd){
	//if (kp < 0 || ki < 0 || kd < 0) return;

	if(direction == DIRECTION_REVERSE){
		this->kp = (0 - kp);
		this->ki = (0 - ki);
		this->kd = (0 - kd);
	}
	else {
		this->kp = kp;
		this->ki = ki;
		this->kd = kd;
	}
}

void PID::setOutputLimits(float min, float max){
	if (min >= max) return;

	outMin = min;
	outMax = max;
}

void PID::setDirection(uint8_t direction){
	if(direction != this->direction){
		kp = (0 - kp);
		ki = (0 - ki);
		kd = (0 - kd);
	}
	this->direction = direction;
}

void PID::reset(uint32_t time){
	integratedError = 0;
	lastMeasured = 0;
	lastTime = time;
}
