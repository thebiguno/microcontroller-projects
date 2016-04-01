#ifndef MAHONY_H
#define MAHONY_H

#include "IMU.h"

namespace digitalcave {
	class Mahony : public IMU {
		public:
			//Constructor
			Mahony(float kp, float ki, uint32_t time);
			~Mahony();

			// Perform the actual calculations.  It should be called repeatedly in the main loop. 
			// No return from this function; call getEuler to get the updated RPY angles
			void compute(vector_t accel, vector_t gyro, vector_t mag, uint8_t armed, uint32_t time);
			void compute(vector_t accel, vector_t gyro, uint8_t armed, uint32_t time);
			
			float getKp(){ return kp; }
			void setKp(float kp) { this->kp = kp; }
			float getKi(){ return ki; }
			void setKi(float ki) { this->ki = ki; }
			
		private:
			//Tuning variables
			float kp;
			float ki;
			
			float integralFBx;	// integral error terms scaled by Ki
			float integralFBy;
			float integralFBz;

	};
}
#endif

