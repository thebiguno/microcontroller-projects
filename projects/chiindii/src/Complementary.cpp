/**********************************************************************************************
 * Basic Complementary Filter class.
 * by Warren Janssens <warren.janssens@gmail.com>
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include <stdio.h>

#include "Complementary.h"
#include "lib/usb/serial.h"

using namespace digitalcave;

Complementary::Complementary(double tau, uint16_t period, uint32_t time) :
		tau(tau),
		period(period),
		lastTime(0),
		result(0){
	if (time > period) lastTime = time - period;
	else lastTime = 0;
}

uint8_t Complementary::compute(double rate, double angle, double* output, uint32_t time){
	uint32_t currentPeriod = time - lastTime;

	if (currentPeriod >= period){
		double periodSec = currentPeriod / 1000.0;	//Period in seconds; used to normalize ki / kd values across different periods
		double a = tau / (tau + periodSec);

		result = a * (result + rate * periodSec) + (1 - a) * angle;

		//Remember some variables for next time
		lastTime = time;
		
		*output = result;
		
		return true;
	}

	return false;
}


void Complementary::setTau(double tau) {
	this->tau = tau;
}
	
void Complementary::setPeriod(uint16_t period){
	this->period = period;
}

double Complementary::getTau() {
	return tau;
}
