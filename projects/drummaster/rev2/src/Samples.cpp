#include "Samples.h"

using namespace digitalcave;

Samples::Samples():
	samples {
		Sample(0),
		Sample(1),
		Sample(2),
		Sample(3),
		Sample(4),
		Sample(5),
		Sample(6),
		Sample(7),
		Sample(8),
		Sample(9),
		Sample(10),
		Sample(11)
	}
{
	//Nothing to see here...
}


Sample* Samples::findAvailableSample(){
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){		//TODO
		if (samples[i].isPlaying() == 0){
			return &(samples[i]);
		}
		else if (samples[i].positionMillis() > oldestSamplePosition){
			oldestSamplePosition = samples[i].positionMillis();
			oldestSample = i;
		}
	}
	samples[oldestSample].stop();
	return &(samples[oldestSample]);
}