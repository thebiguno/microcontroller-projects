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

void Display::write_time(dc_time_t time, uint8_t flash_field){
	if (time.mode == TIME_MODE_24){
		//Write the hours
		if (flash_field != TIME_FIELD_HOUR || flash_timer > FLASH_TIMER_ON){
			snprintf(temp, sizeof(temp), "%02d:", time.hour);
			buffer.write_string(temp, font_5x8, 0, 0);
		}

		//Write the colon
		buffer.write_string(":", font_5x8, 12, 0);

		//Write the minutes
		if (flash_field != TIME_FIELD_MINUTE || flash_timer > FLASH_TIMER_ON){
			snprintf(temp, sizeof(temp), "%02d", time.minute);
			buffer.write_string(temp, font_5x8, 15, 0);
		}
	}
	else {
		//Write the hours
		if (flash_field != TIME_FIELD_HOUR || flash_timer > FLASH_TIMER_ON){
			snprintf(temp, sizeof(temp), "%d", time.hour);
			buffer.write_string(temp, font_5x8, (time.hour >= 10 ? 0 : 6), 0);	//Variable width fonts don't allow for using printf's spacing, so we do it manually
		}

		//Write the colon
		buffer.write_string(":", font_5x8, 12, 0);

		//Write the minutes
		if (flash_field != TIME_FIELD_MINUTE || flash_timer > FLASH_TIMER_ON){
			snprintf(temp, sizeof(temp), "%02d", time.minute);
			buffer.write_string(temp, font_5x8, 15, 0);
		}

		//Write AM / PM
		buffer.write_char((time.mode == TIME_MODE_AM ? 'A' : 'P'), font_5x8, 29, 0);
	}
}

void Display::write_date(dc_time_t time, uint8_t flash_field){
	//Write the day of week
	switch (time_get_day_of_week(time)){
		case 0: buffer.write_string("Su", font_5x8, 0, 0); break;
		case 1: buffer.write_string("Mo", font_5x8, 0, 0); break;
		case 2: buffer.write_string("Tu", font_5x8, 0, 0); break;
		case 3: buffer.write_string("We", font_5x8, 0, 0); break;
		case 4: buffer.write_string("Th", font_5x8, 0, 0); break;
		case 5: buffer.write_string("Fr", font_5x8, 0, 0); break;
		case 6: buffer.write_string("Sa", font_5x8, 0, 0); break;
	}

	//Write the month
	if (flash_field != TIME_FIELD_MONTH || flash_timer > FLASH_TIMER_ON){
		snprintf(temp, sizeof(temp), "%02d", time.month);
		buffer.write_string(temp, font_5x8, 8, 0);
	}

	//Write the day of month
	if (flash_field != TIME_FIELD_DAY_OF_MONTH || flash_timer > FLASH_TIMER_ON){
		snprintf(temp, sizeof(temp), "%02d", time.day_of_month);
		buffer.write_string(temp, font_5x8, 21, 0);
	}

}

void Display::update(State state){
	buffer.clear();

	flash_timer = (flash_timer + 1) & 0x0F;

	uint8_t mode = state.get_mode();
	uint8_t menu_item = state.get_menu_item();
	uint8_t edit_item = state.get_edit_item();

	char temp[10];

	if (mode == MODE_TIME){
		dc_time_t time = state.get_time();
		if (edit_item == 0){
			write_time(time, 0);
		}
		else if (edit_item == 1){
			write_date(time, 0);
		}
		else if (edit_item == 2){
			buffer.write_string("TODO", font_3x5, 8, 1);
		}
	}
	else if (mode == MODE_MENU){
		if (menu_item == MENU_SET_ALARM_1){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("1", font_5x8, 25, 0);
		}
		else if (menu_item == MENU_SET_ALARM_2){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("2", font_5x8, 25, 0);
		}
		else if (menu_item == MENU_SET_ALARM_3){
			buffer.write_string("SET", font_3x5, 2, 2);
			buffer.write_string("0", font_icon, 16, 0);			//0 is alarm icon
			buffer.write_string("3", font_5x8, 25, 0);
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

			if (edit_item == 0){
				write_time(alarm.time, TIME_FIELD_HOUR);
			}
			else if (edit_item == 1){
				write_time(alarm.time, TIME_FIELD_MINUTE);
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
				buffer.write_string("3", font_icon, 0, 0);			//Icon 3 is brightness
				snprintf(temp, sizeof(temp), "%d", alarm.lamp_speed);
				buffer.write_string(temp, font_5x8, (alarm.lamp_speed < 10 ? 15 : 9), 0);
				buffer.write_string("min", font_3x5, 21, 3);
			}
			else if (edit_item == 10){
				buffer.write_string("2", font_icon, 0, 0);			//Icon 2 is music
				snprintf(temp, sizeof(temp), "%d", alarm.lamp_speed);
				buffer.write_string(temp, font_5x8, (alarm.lamp_speed < 10 ? 15 : 9), 0);
				buffer.write_string("min", font_3x5, 21, 3);
			}
			else if (edit_item == 11){
				snprintf(temp, sizeof(temp), "Index %d", alarm.music_index);
				buffer.write_string(temp, font_3x5, 0, 0);
			}
		}
		else if (menu_item == MENU_SET_TIME){
			dc_time_t time = state.get_time();

			if (edit_item == 0){
				if (flash_timer > FLASH_TIMER_ON){
					snprintf(temp, sizeof(temp), "%04d", time.year);
					buffer.write_string(temp, font_5x8, 4, 0);
				}
			}
			else if (edit_item == 1){
				write_date(time, TIME_FIELD_MONTH);
			}
			else if (edit_item == 2){
				write_date(time, TIME_FIELD_DAY_OF_MONTH);
			}
			else if (edit_item == 3){
				write_time(time, TIME_FIELD_HOUR);
			}
			else if (edit_item == 4){
				write_time(time, TIME_FIELD_MINUTE);
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

	display.set_brightness(state.get_display_brightness());
}
