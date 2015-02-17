#include <avr/io.h>
#include <util/delay.h>

#include "lib/Hd44780/CharDisplay.h"
#include "lib/Hd44780/Hd44780_Direct.h"

#include "Channel.h"
#include "State.h"

using namespace digitalcave;

#ifndef CHANNEL_COUNT
#define CHANNEL_COUNT					4
#endif

#if CHANNEL_COUNT > 6
#error The software does not currently support more than 6 channels (not enough ADCs on an ATmega32u4).
#endif

#ifndef DISPLAY_ROWS
#define DISPLAY_ROWS					2
#endif

#ifndef DISPLAY_COLS
#define DISPLAY_COLS					16
#endif

#define MENU_COUNT						3

static Hd44780_Direct hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7);
static CharDisplay char_display(&hd44780, DISPLAY_ROWS, DISPLAY_COLS);
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
//You can add more channels if desired...
};

void read_power_supply(){
	for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
		channels[i].sample_actual();
	}
}

void update_char_display(){
	if (state.get_state() == STATE_LOCKED){
		char temp[20];
		uint8_t channel = state.get_scroll_value() >> 1;
		if (channel > (CHANNEL_COUNT - DISPLAY_ROWS)){
			channel = (CHANNEL_COUNT - DISPLAY_ROWS);
		}

		for(uint8_t row = 0; row < DISPLAY_ROWS; row++){
			channels[channel].to_string((state.get_state() == STATE_LOCKED), temp, DISPLAY_COLS + 1);
			char_display.write_text(row, 0, temp, DISPLAY_COLS);
			if ((channel == (state.get_scroll_value() >> 1)) && (state.get_state() == STATE_LOCKED)){
				char_display.set_cursor_position(row, state.get_scroll_value() & 0x01 ? 9 : 1);
			}
			channel++;
		}
	}
	
	char_display.refresh();
}

int main (void){
	//Debugging lights
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	
	//Display init
	char_display.clear();
	char_display.write_text(1, 2, "Testing 1,2,3", 13);
	char_display.refresh();
	_delay_ms(1000);
	
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
		
		//Refresh the char_display
		update_char_display();
	}
}

