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
		snprintf(temp, sizeof(temp), "%02d:%02d", time.hour, time.minute);
		buffer.write_string(temp, font_clockface, 0, 0);
	}
	else if (state.get_state() == STATE_MENU){
		if (state.get_menu_item() == MENU_SET_ALARM){
			buffer.write_string("Alarm", font_3x5, 0, 0);
		}
		else if (state.get_menu_item() == MENU_SET_TIME){
			buffer.write_string("Set Time", font_3x5, 0, 0);
		}
	}

	display.write_buffer(buffer.get_data());
}
