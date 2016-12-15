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
			float kp, ki, kd;

			//Direction (normal or reverse)
			uint8_t direction;

			//Output limits
			float outMin, outMax;

			//State variables
			uint32_t lastTime;
			float integratedError;
			float lastMeasured;

		public:

			//Constructor
			PID(float kp, float ki, float kd, uint8_t direction, uint32_t time);

			// Perform the actual PID calculation.  It should be called on a regular frequency in the main loop.
			// If this is called more frequently than once per millisecond, accuracy will decrease and may cause errors.
			// Returns the process variable
			float compute(float setPoint, float measured, uint32_t time);

			//Clamps the output to a specific range. (0-255 by default)
			void setOutputLimits(float min, float max);

			//Get / Set PID tunings
			void getTunings(float* kp, float* ki, float* kd);
			void setTunings(float kp, float ki, float kd);

			//Sets the direction.  Normal means output will increase if error is positive.  Reverse is the opposite.
			void setDirection(uint8_t direction);

			//Reset all internal state
			void reset(uint32_t time);
	};
}
#endif
