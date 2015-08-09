#ifndef SAMPLES_H
#define SAMPLES_H

#include "Sample.h"

#define SAMPLE_COUNT				12

namespace digitalcave {

	class Samples {
		private:
			Sample samples[SAMPLE_COUNT];
			
		public:
			Samples();
			Sample* findAvailableSample();
			static void setMasterVolume(double volume);

	};
	
}

#endif