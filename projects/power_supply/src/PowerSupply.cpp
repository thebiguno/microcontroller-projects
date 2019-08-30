#include "PowerSupply.h"

using namespace digitalcave;

Display display;
State state;
// SerialUSB serial;

Channel channels[CHANNEL_COUNT] = {
#if CHANNEL_COUNT > 0
	Channel(0, DAC_ADDRESS_0, DAC_CHANNEL_0, DAC_CHANNEL_1, ADC_CHANNEL_0, ADC_CHANNEL_1, 12500, 5000),
#endif
#if CHANNEL_COUNT > 1
	Channel(1, DAC_ADDRESS_0, DAC_CHANNEL_2, DAC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, 12500, 5000),
#endif
#if CHANNEL_COUNT > 2
	Channel(2, DAC_ADDRESS_1, DAC_CHANNEL_0, DAC_CHANNEL_1, ADC_CHANNEL_6, ADC_CHANNEL_7, 12500, 1500),
#endif
#if CHANNEL_COUNT > 3
	//TODO I have some bad hardware (my DAC1 channel 3 is broken), so I shuffle the channels around.  Don't do this in real life.
	Channel(3, DAC_ADDRESS_2, DAC_CHANNEL_0, DAC_CHANNEL_1, ADC_CHANNEL_8, ADC_CHANNEL_9, -12500, -1500),
#endif
//You can add more channels if desired... just make sure there are enough ADCs.
};

int main(){
	timer_init();

	//Main program loop
	while (1){
		//Read the current, actual values
		for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
			channels[i].sample_actual();
		}

		//Check for state updates
		state.poll();

		//Refresh the display
		display.update(state);
	}
}
