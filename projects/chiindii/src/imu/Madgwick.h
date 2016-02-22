#ifndef COMPLEMENTARY_h
#define COMPLEMENTARY_h

#include <stdint.h>
#include <math.h>
#include <dcmath.h>
#include <dctypes.h>

namespace digitalcave {
	class Madgwick {
		private:
			//Tuning variables
			double beta;
		
			//State variables
			uint32_t lastTime;
			double q0;
			double q1;
			double q2;
			double q3;  // quaternion of sensor frame relative to auxiliary frame

		
		public:

			//Constructor
			Madgwick(double beta, uint32_t time);

			// Perform the actual calculations.  It should be called repeatedly in the main loop. 
			// No return from this function; call getEuler to get the updated RPY angles
			void compute(vector_t accel, vector_t gyro, uint32_t time);
			
			//Returns the euler angles (in radians) for Roll, Pitch, and Yaw
			vector_t getEuler();
			
			void setBeta(double beta);
			double getBeta();
	};
}
#endif

