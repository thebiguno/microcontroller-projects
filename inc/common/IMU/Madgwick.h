#ifndef MADGWICK_H
#define MADGWICK_H

#include "IMU.h"

namespace digitalcave {
	class Madgwick : public IMU {
		public:
			//Constructor
			Madgwick(float beta, uint32_t time);
			~Madgwick();

			// Perform the actual calculations.  It should be called repeatedly in the main loop.
			// No return from this function; call getEuler to get the updated RPY angles
			void compute(vector_t accel, vector_t gyro, vector_t mag, uint8_t armed, uint32_t time);
			void compute(vector_t accel, vector_t gyro, uint8_t armed, uint32_t time);

			float getBeta(){ return beta; }
			void setBeta(float beta) { this->beta = beta; }

		private:
			//Tuning variables
			float beta;
	};
}
#endif
