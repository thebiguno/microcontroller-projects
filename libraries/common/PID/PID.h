#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	1.1.1

#include <stdint.h>

//Constants used in some of the functions below
#define DIRECTION_NORMAL	0
#define DIRECTION_REVERSE	1

class PID {
	private:
		//Pid tuning variables
		double kp, ki, kd;
		
		//Direction (normal or reverse)
		uint8_t direction;
		
		//Period of computation
		uint16_t period;
		
		//Output limits
		double outMin, outMax;
		
		//State variables
		uint32_t lastTime;
		double integratedError;
		double lastMeasured;
		
	public:

		//Constructor
		PID(double kp, double ki, double kd, uint8_t direction, uint32_t period, uint32_t time);

		// Perform the actual PID calculation.  It should be called repeatedly in the main loop. 
		// ON/OFF and calculation frequency can be set using setMode and setSampleTime respectively.
		// Returns non-zero if new output is computed, zero otherwise.  Passes the output
		// back by reference.
		uint8_t compute(double setPoint, double measured, double* output, uint32_t time);

		//Clamps the output to a specific range. (0-255 by default)
		void setOutputLimits(double min, double max);
	
		//Change PID tunings
		void setTunings(double kp, double ki, double kd);
		
		//Sets the direction.  Normal means output will increase if error is positive.  Reverse is the opposite.
		void setDirection(uint8_t direction);

		// Set the period (ms) at which the PID calculation is performed.
		void setPeriod(uint16_t newPeriod);
};
#endif

