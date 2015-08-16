#ifndef SAMPLES_H
#define SAMPLES_H

#include "Sample.h"

#define SAMPLE_COUNT				12

namespace digitalcave {

	class Samples {
		private:
			static AudioControlSGTL5000 control;
			static uint8_t controlEnabled;
			
			Sample samples[SAMPLE_COUNT];
			
		public:
			Samples();
			Sample* findAvailableSample(uint8_t channel, uint8_t volume);
			static void setMasterVolume(double volume);

	};
	
}

#endif