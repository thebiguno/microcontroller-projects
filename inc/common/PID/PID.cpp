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

PID::PID(double kp, double ki, double kd, uint8_t direction, uint32_t period, uint32_t time) :
	integratedError(0),
	lastMeasured(0)
{
	setOutputLimits(0, 255);
	setPeriod(period);
	setDirection(direction);
	setTunings(kp, ki, kd);

	if (time > period) lastTime = time - period;
	else lastTime = 0;
}

uint8_t PID::compute(double setPoint, double measured, double* output, uint32_t time){
	uint32_t currentPeriod = time - lastTime;
	if (currentPeriod >= period){
		double periodSec = currentPeriod / 1000.0;	//Period in seconds; used to normalize ki / kd values across different periods
		
		//Compute all the working error variables
		double error = setPoint - measured;
		integratedError += (ki * periodSec * error);

		if (integratedError > outMax) integratedError = outMax;
		else if (integratedError < outMin) integratedError = outMin;
		
		double derivative = (measured - lastMeasured);
 
		// Compute PID Output
		double result = kp * error + integratedError - kd / periodSec * derivative;
		
		if (result > outMax) result = outMax;
		else if(result < outMin) result = outMin;
		*output = result;
		
		//Remember some variables for next time
		lastMeasured = measured;
		lastTime = time;
		return true;
	}
	return false;
}


void PID::setTunings(double kp, double ki, double kd){
	if (kp < 0 || ki < 0 || kd < 0) return;

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
	
void PID::setPeriod(uint16_t period){
	this->period = period;
}
void PID::setOutputLimits(double min, double max){
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

double PID::getKp() {
	return kp;
}

double PID::getKi() {
	return ki;
}

double PID::getKd() {
	return kd;
}
