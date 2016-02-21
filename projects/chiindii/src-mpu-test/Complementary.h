#ifndef COMPLEMENTARY_h
#define COMPLEMENTARY_h

#include <stdint.h>

namespace digitalcave {
	class Complementary {
		private:
			//Tuning variables
			double tau;
		
			//State variables
			uint32_t lastTime;
			double result;
		
		public:

			//Constructor
			Complementary(double tau, uint32_t time);

			// Perform the actual Complementary calculation.  It should be called repeatedly in the main loop. 
			// ON/OFF and calculation frequency can be set using setMode and setSampleTime respectively.
			// Returns non-zero if new output is computed, zero otherwise.  Passes the output
			// back by reference.
			double compute(double rate, double angle, uint32_t time);
			
			void setTau(double tau);
			double getTau();
	};
}
#endif

