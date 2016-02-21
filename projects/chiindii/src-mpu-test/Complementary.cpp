/**********************************************************************************************
 * Basic Complementary Filter class.
 * by Warren Janssens <warren.janssens@gmail.com>
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include <stdio.h>

#include "Complementary.h"

using namespace digitalcave;

Complementary::Complementary(double tau, uint32_t time) :
		tau(tau),
		lastTime(time),
		result(0)
{
}

double Complementary::compute(double rate, double angle, uint32_t time){
	uint32_t currentPeriod = time - lastTime;
	double periodSec = currentPeriod / 1000.0;	//Period in seconds; used to normalize ki / kd values across different periods
	double a = tau / (tau + periodSec);

	result = a * (result + rate * periodSec) + (1 - a) * angle;

	//Remember some variables for next time
	lastTime = time;
	
	return result;
}


void Complementary::setTau(double tau) {
	this->tau = tau;
}
	
double Complementary::getTau() {
	return tau;
}
