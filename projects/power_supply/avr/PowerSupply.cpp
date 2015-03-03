#include "PowerSupply.h"

using namespace digitalcave;

static Display display;

State state;
Channel channels[CHANNEL_COUNT] = {
#if CHANNEL_COUNT > 0
	Channel(0x60, 0, 0.0, 12.5, 0, 0.0, 2.0, 1),
#endif
#if CHANNEL_COUNT > 1
	Channel(0x60, 1, -12.5, 0.0, 4, 0.0, 2.0, 5),
#endif
#if CHANNEL_COUNT > 2
	Channel(0x60, 0, 0.0, 12.0, 6, 0.0, 5.0, 7),
#endif
#if CHANNEL_COUNT > 3
	Channel(0x60, 1, -12.5, 0.0, 8, 0.0, 1.0, 9),
#endif
#if CHANNEL_COUNT > 4
	Channel(0x60, 0, -5.0, 0.0, 10, 0.0, 2.0, 11),
#endif
#if CHANNEL_COUNT > 5
	Channel(0x60, 1, -3.5, 0.0, 12, 0.0, 10.0, 13),
#endif
//You can add more channels if desired... just make sure there are enough ADCs.
};

int main(){
	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
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

