/**********************************************************************************************
 * Basic Complementary Filter class.
 * by Warren Janssens <warren.janssens@gmail.com>
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include "Complementary.h"

using namespace digitalcave;

Complementary::Complementary(double tau, uint32_t period, uint32_t time){
	setPeriod(period);
	setTuning(tau);

	if (time > period) lastTime = time - period;
	else lastTime = 0;
}

uint8_t Complementary::compute(double rate, double angle, double* output, uint32_t time){
	uint32_t currentPeriod = time - lastTime;
	if (currentPeriod >= period){
		double periodSec = currentPeriod / 1000.0;	//Period in seconds; used to normalize ki / kd values across different periods
		double a = tau / (tau + periodSec);
		result = a * (result + rate * periodSec) + (1 - a) * angle;
 
		*output = result;
		
		//Remember some variables for next time
		lastTime = time;
		return true;
	}
	return false;
}


void Complementary::setTuning(double tau) {
	this->tau = tau;
}
	
void Complementary::setPeriod(uint16_t period){
	this->period = period;
}
