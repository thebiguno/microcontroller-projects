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
	char_display.mark_dirty();
	
	if (state.get_state() == STATE_LOCKED){
		char buffer[DISPLAY_COLS + 1];
		uint8_t channel = state.get_scroll_channel();
		if (CHANNEL_COUNT > DISPLAY_ROWS && channel > (CHANNEL_COUNT - DISPLAY_ROWS)){
			channel = (CHANNEL_COUNT - DISPLAY_ROWS);
		}
		else if (CHANNEL_COUNT < DISPLAY_ROWS){
			channel = 0;	
		}

		for(uint8_t row = 0; row < DISPLAY_ROWS && row < CHANNEL_COUNT; row++){
			double voltage = state.get_state() == STATE_LOCKED ? channels[channel].get_voltage_actual() / 1000.0 : channels[channel].get_voltage_setpoint() / 1000.0;
			double current = state.get_state() == STATE_LOCKED ? channels[channel].get_current_actual() / 1000.0 : channels[channel].get_current_setpoint() / 1000.0;

			snprintf(buffer, DISPLAY_COLS + 1, "Ch %d %+6.2fV %5.2fA   ", channel + 1, voltage, current);
			char_display.write_text(row, 0, buffer, DISPLAY_COLS);

			channel++;
		}
		
		for (uint8_t row = CHANNEL_COUNT; row < DISPLAY_ROWS; row++){
			char_display.write_text(row, 0, "                    ", DISPLAY_COLS);
		}
	}
	else if (state.get_state() == STATE_EDIT_ITEM){
		char buffer[DISPLAY_COLS + 1];
		uint8_t channel = state.get_scroll_channel();
		
		snprintf(buffer, DISPLAY_COLS + 1, "   Channel %d         ", channel + 1);
		char_display.write_text(0, 0, buffer, DISPLAY_COLS);
		
		snprintf(buffer, DISPLAY_COLS + 1, "Set  %+6.2fV %5.2fA     ", channels[channel].get_voltage_setpoint() / 1000.0, channels[channel].get_current_setpoint() / 1000.0);
		char_display.write_text(1, 0, buffer, DISPLAY_COLS);

		snprintf(buffer, DISPLAY_COLS + 1, "Actl.%+6.2fV %5.2fA     ", channels[channel].get_voltage_actual() / 1000.0, channels[channel].get_current_actual() / 1000.0);
		char_display.write_text(2, 0, buffer, DISPLAY_COLS);

		char_display.write_text(3, 0, "                    ", DISPLAY_COLS);
		
		char_display.write_text(1, 4, 0x7e);
		char_display.write_text(1, 19, 0x7f);
	}
	//TODO Add menu support here...
	
	char_display.refresh();
}

void Display::force_reset(){
	char_display.clear();
}