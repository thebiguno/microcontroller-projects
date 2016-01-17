#ifndef COMPLEMENTARY_h
#define COMPLEMENTARY_h

#include <stdint.h>

namespace digitalcave {
	class Complementary {
		private:
			//Tuning variables
			double tau;
		
			//Period of computation
			uint16_t period;
		
			//State variables
			uint32_t lastTime;
			double result;
		
		public:

			//Constructor
			Complementary(double tau, uint32_t period, uint32_t time);

			// Perform the actual Complementary calculation.  It should be called repeatedly in the main loop. 
			// ON/OFF and calculation frequency can be set using setMode and setSampleTime respectively.
			// Returns non-zero if new output is computed, zero otherwise.  Passes the output
			// back by reference.
			uint8_t compute(double rate, double angle, double* output, uint32_t time);
			
			// Set the period (ms) at which the PID calculation is performed.
			void setPeriod(uint16_t newPeriod);
			
			void setTau(double tau);
			double getTau();
	};
}
#endif

