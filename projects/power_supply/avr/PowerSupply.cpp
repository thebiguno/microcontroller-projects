#include "PowerSupply.h"

using namespace digitalcave;

Display display;

State state;
Channel channels[CHANNEL_COUNT] = {
#if CHANNEL_COUNT > 0
	Channel(DAC_ADDRESS_0, DAC_CHANNEL_0, DAC_CHANNEL_1, 12500, 5000, ADC_CHANNEL_0, ADC_CHANNEL_1),
#endif
#if CHANNEL_COUNT > 1
	Channel(DAC_ADDRESS_0, DAC_CHANNEL_2, DAC_CHANNEL_3, -12500, 5000, ADC_CHANNEL_4, ADC_CHANNEL_5),
#endif
#if CHANNEL_COUNT > 2
	Channel(DAC_ADDRESS_1, DAC_CHANNEL_0, DAC_CHANNEL_1, 12000, 1000, ADC_CHANNEL_6, ADC_CHANNEL_7),
#endif
#if CHANNEL_COUNT > 3
	Channel(DAC_ADDRESS_1, DAC_CHANNEL_2, DAC_CHANNEL_3, -12000, 500, ADC_CHANNEL_8, ADC_CHANNEL_9),
#endif
#if CHANNEL_COUNT > 4
	Channel(DAC_ADDRESS_2, DAC_CHANNEL_0, DAC_CHANNEL_1, -5000, 2000, ADC_CHANNEL_10, ADC_CHANNEL_11),
#endif
#if CHANNEL_COUNT > 5
	Channel(DAC_ADDRESS_2, DAC_CHANNEL_2, DAC_CHANNEL_3, -3500, 10000, ADC_CHANNEL_12, ADC_CHANNEL_13),
#endif
//You can add more channels if desired... just make sure there are enough ADCs.
};

int main(){
	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
	twi_init();
	usb_init();
	
	//Main program loop
	while (1){
		//Read the current, actual values
		for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
			channels[i].sample_actual();
		}
		
		//Check if we are connected via USB
		if (usb_configured()){
			usb_send_actual_values();
			usb_check_for_updates();
		}
		
		//Check for state updates
		state.poll();
		
		//Refresh the display
		display.update(state);
	}
}
