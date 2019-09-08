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

	uint8_t mode = state.get_mode();
	uint8_t menu_item = state.get_menu_item();
	uint8_t edit_item = state.get_edit_item();

	char temp[10];

	if (mode == MODE_TIME){
		dc_time_t time = state.get_time();
		if (edit_item == 0){
			//Time - default state
			if (time.mode == TIME_MODE_24){
				snprintf(temp, sizeof(temp), "%02d:%02d", time.hour, time.minute);
				buffer.write_string(temp, font_clockface, 0, 0);
			}
			else {
				snprintf(temp, sizeof(temp), "%d:%02d %c", time.hour, time.minute, (time.mode == TIME_MODE_AM ? 'A' : 'P'));
				buffer.write_string(temp, font_clockface, (time.hour >= 10 ? 0 : 6), 0);	//Variable width fonts don't allow for using printf's spacing, so we do it manually
			}
		}
		else if (edit_item == 1){
			//Date
			char day_of_week;
			switch (time_get_day_of_week(time)){
				case 0: day_of_week = 'U'; break;
				case 1: day_of_week = 'M'; break;
				case 2: day_of_week = 'T'; break;
				case 3: day_of_week = 'W'; break;
				case 4: day_of_week = 'H'; break;
				case 5: day_of_week = 'F'; break;
				case 6: day_of_week = 'S'; break;
				default: day_of_week = ' '; break;
			}
			snprintf(temp, sizeof(temp), "%02d.%02d.%02d", time.year - 2000, time.month, time.day_of_month);
			buffer.write_string(temp, font_3x5, 0, 1);
			snprintf(temp, sizeof(temp), "%c", day_of_week);
			buffer.write_string(temp, font_3x5, 29, 1);
		}
		else if (edit_item == 2){
			buffer.write_string("TODO", font_3x5, 8, 1);
		}
	}
	else if (mode == MODE_MENU){
		if (menu_item == MENU_SET_ALARM_1){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("1", font_clockface, 25, 0);
		}
		else if (menu_item == MENU_SET_ALARM_2){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("2", font_clockface, 25, 0);
		}
		else if (menu_item == MENU_SET_ALARM_3){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("3", font_clockface, 25, 0);
		}
		else if (menu_item == MENU_SET_TIME){
			buffer.write_string("SET", font_3x5, 5, 2);
			buffer.write_string("1", font_icon, 19, 0);			//1 is clock icon
		}
	}
	else if (mode == MODE_EDIT){
		if (menu_item == MENU_SET_ALARM_1 || menu_item == MENU_SET_ALARM_2 || menu_item == MENU_SET_ALARM_3){
			//Find alarm index
			uint8_t alarm_index;
			if (menu_item == MENU_SET_ALARM_1){
				alarm_index = 0;
			}
			else if (menu_item == MENU_SET_ALARM_2){
				alarm_index = 1;
			}
			else if (menu_item == MENU_SET_ALARM_3){
				alarm_index = 2;
			}

			alarm_t alarm = state.get_alarm(alarm_index);

			if (edit_item < 2){
				//Draw the underline depending on what we are editing
				if (edit_item == 0){
					buffer.write_string("__", font_3x5, 0, 2);		//Hour
					buffer.write_string("_", font_3x5, 1, 2);
				}
				else if (edit_item == 1){
					buffer.write_string("__", font_3x5, 10, 2);		//Minute
					buffer.write_string("_", font_3x5, 11, 2);
				}

				dc_time_t time = alarm.time;
				if (time.mode == TIME_MODE_24){
					snprintf(temp, sizeof(temp), "%02d:%02d", time.hour, time.minute);
					buffer.write_string(temp, font_3x5, 0, 0);
				}
				else {
					snprintf(temp, sizeof(temp), "%d:%02d", time.hour, time.minute);
					buffer.write_string(temp, font_3x5, time.hour >= 10 ? 0 : 4, 0);
					snprintf(temp, sizeof(temp), "%c", (time.mode == TIME_MODE_AM ? 'A' : 'P'));
					buffer.write_string(temp, font_3x5, 29, 0);
				}
			}
			else if (edit_item < 9){
				for (uint8_t i = 0; i < 7; i++){
					if (alarm.enabled & _BV(i)){
						buffer.write_string("_", font_3x5, i * 4, 3);
					}
					if ((edit_item - 2) == i){
						buffer.write_string("_", font_3x5, i * 4, 2);
					}
				}

				buffer.write_string("UMTWHFS", font_3x5, 0, 0);
			}
			else if (edit_item == 9){
				snprintf(temp, sizeof(temp), "Wake %d", alarm.lamp_speed);
				buffer.write_string(temp, font_3x5, 0, 0);
			}
			else if (edit_item == 10){
				snprintf(temp, sizeof(temp), "Music %d", alarm.music_speed);
				buffer.write_string(temp, font_3x5, 0, 0);
			}
			else if (edit_item == 11){
				snprintf(temp, sizeof(temp), "Index %d", alarm.music_index);
				buffer.write_string(temp, font_3x5, 0, 0);
			}
		}
		else if (menu_item == MENU_SET_TIME){
			dc_time_t time = state.get_time();

			//Draw the underline depending on what we are editing
			if (edit_item == 0 || edit_item == 3){
				buffer.write_string("__", font_3x5, 0, 2);		//Year / Hour
				buffer.write_string("_", font_3x5, 1, 2);
			}
			else if (edit_item == 1 || edit_item == 4){
				buffer.write_string("__", font_3x5, 10, 2);		//Month / Minute
				buffer.write_string("_", font_3x5, 11, 2);
			}
			else if (edit_item == 2 || edit_item == 5){
				buffer.write_string("__", font_3x5, 20, 2);		//Day / Second
				buffer.write_string("_", font_3x5, 21, 2);
			}

			if (edit_item < 3){
				char day_of_week;
				switch (time_get_day_of_week(time)){
					case 0: day_of_week = 'U'; break;
					case 1: day_of_week = 'M'; break;
					case 2: day_of_week = 'T'; break;
					case 3: day_of_week = 'W'; break;
					case 4: day_of_week = 'H'; break;
					case 5: day_of_week = 'F'; break;
					case 6: day_of_week = 'S'; break;
					default: day_of_week = ' '; break;
				}
				snprintf(temp, sizeof(temp), "%02d.%02d.%02d", time.year - 2000, time.month, time.day_of_month);
				buffer.write_string(temp, font_3x5, 0, 0);
				snprintf(temp, sizeof(temp), "%c", day_of_week);
				buffer.write_string(temp, font_3x5, 29, 0);
			}
			else if (edit_item < 6){
				if (time.mode == TIME_MODE_24){
					snprintf(temp, sizeof(temp), "%02d:%02d:%02d", time.hour, time.minute, time.second);
					buffer.write_string(temp, font_3x5, 0, 0);
				}
				else {
					snprintf(temp, sizeof(temp), "%2d:%02d:%02d", time.hour, time.minute, time.second);
					buffer.write_string(temp, font_3x5, 0, 0);
					snprintf(temp, sizeof(temp), "%c", (time.mode == TIME_MODE_AM ? 'A' : 'P'));
					buffer.write_string(temp, font_3x5, 29, 0);
				}
			}
			else {
				if (time.mode == TIME_MODE_24){
					buffer.write_string("Mode: 24", font_3x5, 0, 0);
				}
				else {
					buffer.write_string("Mode: 12", font_3x5, 0, 0);
				}
			}
		}
	}

	display.write_buffer(buffer.get_data());

	static uint8_t last_display_brightness = 0;
	if (last_display_brightness != state.get_display_brightness()){
		last_display_brightness = state.get_display_brightness();
		display.set_brightness(last_display_brightness);
	}
}
