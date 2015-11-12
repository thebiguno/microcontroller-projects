// A stupidly simple (and probably stupidly inaccurate) PID output simulation for use with 
// the PID class.  Compile / run with the command
// g++ test.cxx PID.cpp ; ./a.out

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#import "PID.h"

int main(){
	PID pid(1, 3, 0, DIRECTION_NORMAL, 100, 0);
	
	#define CHANGERATE 0.4;
	
	double output;

	uint32_t time = 0;

	double setPoint = 100;
	double measured = 0;
	
	uint8_t stableLoops = 0;

	while (((setPoint - measured) >= 0.1 || (setPoint - measured) <= -0.1) || stableLoops <= 10){
		
		if ((setPoint - measured) < 0.1 && (setPoint - measured) > -0.1) stableLoops++;
		else stableLoops = 0;
		
		pid.compute(setPoint, measured, &output, time);
		measured += (output - measured) * CHANGERATE;
		time += 100;
		printf("Time: %d\tOutput: %f\tMeasured: %f\n", time, output, measured);
	}
}