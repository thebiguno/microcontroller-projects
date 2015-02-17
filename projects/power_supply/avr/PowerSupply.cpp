#include <avr/io.h>
#include <util/delay.h>

#include "PowerSupply.h"

using namespace digitalcave;

static Display display;
static State state;

Channel channels[CHANNEL_COUNT] = {
#if CHANNEL_COUNT > 0
	Channel(0, 15.0, 0.0, 0, 1.0, 0.0, 1),
#endif
#if CHANNEL_COUNT > 1
	Channel(1, 6.0, 2.0, 4, 0.5, 0.0, 5),
#endif
#if CHANNEL_COUNT > 2
	Channel(2, 12.0, 0.0, 6, 5.0, 0.0, 7),
#endif
#if CHANNEL_COUNT > 3
	Channel(3, 0.0, -12.5, 8, 1.0, 0.0, 9),
#endif
#if CHANNEL_COUNT > 4
	Channel(4, 0.0, -5.0, 10, 2.0, 0.0, 11),
#endif
#if CHANNEL_COUNT > 5
	Channel(5, 0.0, -3.5, 12, 0.0, -10.0, 13),
#endif
//You can add more channels if desired... just make sure there are enough ADCs.
};

int main (void){
	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
	//Main program loop
	while (1){
		//TODO
		//if (usb_configured()){
		//	
		//}

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

