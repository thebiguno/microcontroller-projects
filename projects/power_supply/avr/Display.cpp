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
	if (state.get_state() == STATE_LOCKED || state.get_state() == STATE_EDIT){
		char buffer[DISPLAY_COLS + 1];
		uint8_t channel = state.get_scroll_channel();
		if (CHANNEL_COUNT > DISPLAY_ROWS && channel > (CHANNEL_COUNT - DISPLAY_ROWS)){
			channel = (CHANNEL_COUNT - DISPLAY_ROWS);
		}

		for(uint8_t row = 0; row < DISPLAY_ROWS && row < CHANNEL_COUNT; row++){
			double voltage = state.get_state() == STATE_LOCKED ? channels[channel].get_voltage_actual() / 1000.0 : channels[channel].get_voltage_setpoint() / 1000.0;
			double current = state.get_state() == STATE_LOCKED ? channels[channel].get_current_actual() / 1000.0 : channels[channel].get_current_setpoint() / 1000.0;

			snprintf(buffer, DISPLAY_COLS + 1, "%d %+6.2fV %5.3fA     ", channel + 1, voltage, current);
			char_display.write_text(row, 0, buffer, DISPLAY_COLS);

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
	else if (state.get_state() == STATE_EDIT_ITEM){
		char buffer[DISPLAY_COLS + 1];
		uint8_t channel = state.get_scroll_channel();
		
		snprintf(buffer, DISPLAY_COLS + 1, "   Channel %d         ", channel + 1);
		char_display.write_text(0, 0, buffer, DISPLAY_COLS);
		
		snprintf(buffer, DISPLAY_COLS + 1, "Set  %+6.2fV %5.3fA     ", channels[channel].get_voltage_setpoint() / 1000.0, channels[channel].get_current_setpoint() / 1000.0);
		char_display.write_text(1, 0, buffer, DISPLAY_COLS);

		snprintf(buffer, DISPLAY_COLS + 1, "Meas.%+6.2fV %5.3fA     ", channels[channel].get_voltage_actual() / 1000.0, channels[channel].get_current_actual() / 1000.0);
		char_display.write_text(2, 0, buffer, DISPLAY_COLS);

		char_display.write_text(3, 0, "                    ", DISPLAY_COLS);
		
		if (state.get_scroll_value()){
			char_display.write_text(1, 12, 0x7e);
			char_display.write_text(1, 19, 0x7f);
		}
		else {
			char_display.write_text(1, 5, 0x7e);
			char_display.write_text(1, 12, 0x7f);
		}

	}
	//TODO Add menu support here...
	
	char_display.refresh();
}