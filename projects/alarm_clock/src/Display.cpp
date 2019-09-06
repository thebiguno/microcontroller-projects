#include "Display.h"

using namespace digitalcave;

extern ButtonAVR button;

Display::Display() :
	spi(),
	display(&spi, 4),
	buffer(32, 8)
{
	buffer.clear();
}

void Display::update(State state){
	buffer.clear();

	char temp[32];

	if (state.get_light_brightness()){
		light_set(state.get_light_brightness(), 0);
	}

	if (state.get_state() == STATE_TIME){
		ds3231_time_t time = state.get_time();
		if (time.mode == DS3231_MODE_24){
			snprintf(temp, sizeof(temp), "%02d:%02d", time.hour, time.minute);
		}
		else {
			snprintf(temp, sizeof(temp), "%d:%02d %s", time.hour, time.minute, time.mode == DS3231_MODE_AM ? "A" : "P");
		}
		buffer.write_string(temp, font_clockface, 0, 0);
	}
	else if (state.get_state() == STATE_MENU){
		if (state.get_menu_item() == MENU_SET_ALARM_1){
			buffer.write_string("Alarm 1", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_ALARM_2){
			buffer.write_string("Alarm 2", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_ALARM_3){
			buffer.write_string("Alarm 3", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_TIME){
			buffer.write_string("Set Time", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_12_24){
			buffer.write_string("12H / 24H", font_3x5, 0, 0);
		}
	}
	else if (state.get_state() == STATE_EDIT){
		if (state.get_menu_item() == MENU_SET_ALARM_1){
			buffer.write_string("Alarm 1", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_ALARM_2){
			buffer.write_string("Alarm 2", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_ALARM_3){
			buffer.write_string("Alarm 3", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_TIME){
			buffer.write_string("Set Time", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_12_24){
			ds3231_time_t time = state.get_time();
			if (time.mode == DS3231_MODE_24){
				buffer.write_string("24H", font_3x5, 0, 0);
			}
			else {
				buffer.write_string("12H", font_3x5, 0, 0);
			}
		}
	}

	display.write_buffer(buffer.get_data());
}
