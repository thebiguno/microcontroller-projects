#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	1.1.1

#include <stdint.h>

//Constants used in some of the functions below
#define DIRECTION_NORMAL	0
#define DIRECTION_REVERSE	1

namespace digitalcave {
	class PID {
		private:
			//Pid tuning variables
			double kp, ki, kd;
		
			//Direction (normal or reverse)
			uint8_t direction;
		
			//Output limits
			double outMin, outMax;
		
			//State variables
			uint32_t lastTime;
			double integratedError;
			double lastMeasured;
		
		public:

			//Constructor
			PID(double kp, double ki, double kd, uint8_t direction, uint32_t time);

			// Perform the actual PID calculation.  It should be called on a regular frequency in the main loop.
			// If this is called more frequently than once per millisecond, accuracy will decrease and may cause errors.
			// Returns the process variable
			double compute(double setPoint, double measured, uint32_t time);

			//Clamps the output to a specific range. (0-255 by default)
			void setOutputLimits(double min, double max);
	
			//Change PID tunings
			void setTunings(double kp, double ki, double kd);
		
			//Sets the direction.  Normal means output will increase if error is positive.  Reverse is the opposite.
			void setDirection(uint8_t direction);

			//Reset all internal state
			void reset(uint32_t time);
			
			double getKp();
			double getKi();
			double getKd();
			void setKp(double Kp);
			void setKi(double Ki);
			void setKd(double Kd);
	};
}
#endif

