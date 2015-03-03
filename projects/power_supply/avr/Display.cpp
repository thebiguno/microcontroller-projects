#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];

Display::Display() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7),
	char_display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS) {

	char_display.clear();
	char_display.refresh();
}

void Display::update(State state){
	if (state.get_state() == STATE_LOCKED || state.get_state() == STATE_EDIT || state.get_state() == STATE_EDIT_ITEM){
		char temp[DISPLAY_COLS + 1];
		uint8_t channel = state.get_scroll_channel();
		if (channel > (CHANNEL_COUNT - DISPLAY_ROWS)){
			channel = (CHANNEL_COUNT - DISPLAY_ROWS);
		}

		for(uint8_t row = 0; row < DISPLAY_ROWS && row < CHANNEL_COUNT; row++){
			channels[channel].to_string(row, (state.get_state() == STATE_LOCKED), temp, DISPLAY_COLS + 1);
			char_display.write_text(row, 0, temp, DISPLAY_COLS);

			if (channel == (state.get_scroll_channel())){
				if (state.get_state() == STATE_EDIT){
					char_display.write_text(row, state.get_scroll_value() ? 9 : 1, (char) 0xFF);
				}
				else if (state.get_state() == STATE_EDIT_ITEM){
					if (state.get_scroll_value()){
						char_display.write_text(row, 9, 0x7e);
						char_display.write_text(row, 16, 0x7f);
					}
					else {
						char_display.write_text(row, 1, 0x7e);
						char_display.write_text(row, 9, 0x7f);
					}
				}
			}
			
			channel++;
		}
	}
	//TODO Add menu support here...
	
	char_display.refresh();
}