#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <math.h>
#include <dcmath.h>
#include <dctypes.h>

namespace digitalcave {
	class IMU {
		public:

			//Constructor
			IMU(uint32_t time);
			~IMU();

			//Returns the euler angles (in radians) for Roll, Pitch, and Yaw
			vector_t getEuler();
			
			//Returns acceleration on the Z axis (including gravity).
			float getZAcceleration(vector_t accel);

		protected:
			//State variables
			uint32_t lastTime;
			float q0;
			float q1;
			float q2;
			float q3;  // quaternion of sensor frame relative to auxiliary (earth) frame

	};
}
#endif

