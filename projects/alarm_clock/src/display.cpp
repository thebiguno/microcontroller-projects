#include "display.h"

#define TIME_FIELD_YEAR				0x00
#define TIME_FIELD_MONTH			0x01
#define TIME_FIELD_DAY_OF_MONTH		0x02
#define TIME_FIELD_HOUR				0x03
#define TIME_FIELD_MINUTE			0x04
#define TIME_FIELD_SECOND			0x05
#define TIME_FIELD_DAY_OF_WEEK		0x06

using namespace digitalcave;

static Buffer* display_buffer;

static uint8_t flash_timer = 0;
static int8_t scroll_value = 0;			//Scrolling offset.  Start at 0 and then move negative every time flash_timer resets.
static int8_t scroll_direction = -1;		//Scrolling direction.  Either -1 or 1

static void display_write_time(time_t time, uint8_t flash_field);
static void display_write_date(time_t time, uint8_t flash_field);

//This, in combination with the main loop speed, determines how fast things flash
#define FLASH_TIMER_ON		0x7F
#define NO_FLASH			0xFF

static time_t currentTime;
static tm_t tm;

static char buffer[10];

void display_init(){
	display_buffer = new Buffer(32, 8);
	display_buffer->clear();

	max7219_init(4);
}

void display_update(){
	display_buffer->clear();

	flash_timer = flash_timer + 1;

	uint8_t mode = state_get_mode();
	uint8_t menu_item = state_get_menu_item();
	uint8_t edit_item = state_get_edit_item();

	state_get_time(&currentTime, &tm);

	if (mode == MODE_TIME){
		if (edit_item == EDIT_TIME_TIME){
			scroll_value = 4;
			display_write_time(currentTime, NO_FLASH);
		}
		else if (edit_item == EDIT_TIME_LAMP){
			display_buffer->write_string("3", font_icon, 3, 0);                      //Icon 3 is brightness
			uint8_t brightness = (uint8_t) ((state_get_lamp_brightness() * 99) + 1);
			snprintf(buffer, sizeof(buffer), "%d", brightness);
			display_buffer->write_string(buffer, font_5x8, brightness < 10 ? 23 : (brightness < 100 ? 17 : 12), 0);
		}
		else if (edit_item == EDIT_TIME_MUSIC){
			display_buffer->write_string("2", font_icon, 0, 0);                      //Icon 2 is music
			display_buffer->write_string("VOL", font_3x5, 9, 3);
			snprintf(buffer, sizeof(buffer), "%d", music_get_volume());
			display_buffer->write_string(buffer, font_5x8, (music_get_volume() < 10 ? 27 : 21), 0);
		}
		else if (edit_item == EDIT_TIME_DATE){
			display_write_date(currentTime, NO_FLASH);
		}

	}
	else if (mode == MODE_MENU){
		if (menu_item <= MENU_SET_ALARM_3){		//Alarm 1, 2, or 3
			display_buffer->write_string("SET", font_3x5, 2, 2);
			display_buffer->write_char('0', font_icon, 16, 0);			//0 is alarm icon
			display_buffer->write_char((char) (menu_item + 0x31 - MENU_SET_ALARM_1), font_5x8, 25, 0);
		}
		else if (menu_item == MENU_SET_TIME){
			display_buffer->write_string("SET", font_3x5, 5, 2);
			display_buffer->write_char('1', font_icon, 19, 0);			//1 is clock icon
		}
		else if (menu_item == MENU_DFU){
			display_buffer->write_string("4", font_icon, 0, 0);			//Icon 4 is DFU Upload
			display_buffer->write_string("UPLOAD", font_3x5, 8, 2);
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

			alarm_t alarm = state_get_alarm(alarm_index);

			if (edit_item == 0 || edit_item == 1){
				tm_t tm;
				localtime_r(&currentTime, &tm);
				tm.tm_hour = alarm.hour;
				tm.tm_min = alarm.minute;
				display_write_time(mktime(&tm), edit_item + 3);
			}
			else if (edit_item < 9){
				if (edit_item == 2){
					scroll_value = 0;
				}
				else if (flash_timer & 0x01){
					if (scroll_value > ((edit_item - 2) * -7) + 14 && scroll_value > -16){
						scroll_value--;
					}
				}

				for (uint8_t i = 0; i < 7; i++){
					if (alarm.enabled & _BV(i)){
						//Draw the underline for days that are enabled
						display_buffer->write_char((char) 0x48, font_icon, (i * 7) + scroll_value, 0);
					}
					if (flash_timer > FLASH_TIMER_ON || edit_item != (i + 2)){
						//Write the days
						display_buffer->write_char((char) (i + 0x41), font_icon, (i * 7) + scroll_value, 0);		//In the Icon font, we define Sunday as 'A' (0x41), Monday as 'B', etc.  So (i + 0x41) returns the day of the week.
					}
				}
			}
			else if (edit_item == 9){
				display_buffer->write_string("3", font_icon, 0, 0);			//Icon 3 is brightness
				snprintf(buffer, sizeof(buffer), "%d", alarm.lamp_speed);
				display_buffer->write_string(buffer, font_5x8, (alarm.lamp_speed < 10 ? 15 : 9), 0);
				display_buffer->write_string("min", font_3x5, 21, 3);
			}
			else if (edit_item == 10){
				display_buffer->write_string("2", font_icon, 0, 0);			//Icon 2 is brightness
				snprintf(buffer, sizeof(buffer), "%d", alarm.music_speed);
				display_buffer->write_string(buffer, font_5x8, (alarm.music_speed < 10 ? 15 : 9), 0);
				display_buffer->write_string("min", font_3x5, 21, 3);
			}
			else if (edit_item == 11){
				display_buffer->write_string("2", font_icon, 0, 0);			//Icon 2 is music
				display_buffer->write_string("VOL", font_3x5, 9, 3);
				snprintf(buffer, sizeof(buffer), "%d", alarm.music_volume);
				display_buffer->write_string(buffer, font_5x8, (alarm.music_volume < 10 ? 27 : 21), 0);
			}
		}
		else if (menu_item == MENU_SET_TIME){
			state_get_time(&currentTime, &tm);

			if (edit_item == 0){
				display_write_date(currentTime, TIME_FIELD_YEAR);
			}
			else if (edit_item == 1){
				display_write_date(currentTime, TIME_FIELD_MONTH);
			}
			else if (edit_item == 2){
				display_write_date(currentTime, TIME_FIELD_DAY_OF_MONTH);
			}
			else if (edit_item == 3){
				display_write_time(currentTime, TIME_FIELD_HOUR);
			}
			else if (edit_item == 4){
				display_write_time(currentTime, TIME_FIELD_MINUTE);
			}
		}
		else if (menu_item == MENU_DFU){
			display_buffer->write_string("4", font_icon, 0, 0);			//Icon 4 is DFU Upload
			display_buffer->write_string("CLICK", font_3x5, 10, 2);
		}
	}

	max7219_write_buffer(display_buffer->get_data());

	max7219_set_brightness(state_get_display_brightness());
	if (state_get_display_brightness() == 0){
		display_buffer->clear();
		max7219_write_buffer(display_buffer->get_data());
	}
}


static void display_write_time(time_t time, uint8_t flash_field){
	tm_t tm;
	localtime_r(&time, &tm);
	//
	// strftime(buffer, sizeof(buffer), "%l:%M%p", &tm);
	// display_buffer->write_string(buffer, font_5x8, 0, 0);

	//Write the hours
	if (flash_field != TIME_FIELD_HOUR || flash_timer > FLASH_TIMER_ON){
		uint8_t hour = tm.tm_hour;
		if (hour == 0) hour = 12;
		else if (hour > 12) hour -= 12;
		snprintf(buffer, sizeof(buffer), "%d", hour);
		display_buffer->write_string(buffer, font_5x8, (hour >= 10 ? 0 : 6), 0);	//Variable width fonts don't allow for using printf's spacing, so we do it manually
	}

	//Write the colon
	display_buffer->write_string(":", font_5x8, 12, 0);

	//Write the minutes
	if (flash_field != TIME_FIELD_MINUTE || flash_timer > FLASH_TIMER_ON){
		snprintf(buffer, sizeof(buffer), "%02d", tm.tm_min);
		display_buffer->write_string(buffer, font_5x8, 14, 0);
	}

	//Write AM / PM
	display_buffer->write_char((tm.tm_hour < 12 ? 'A' : 'P'), font_5x8, 28, 0);
}

static void display_write_date(time_t time, uint8_t flash_field){
	tm_t tm;
	localtime_r(&time, &tm);

	//Write the year
	if (flash_field != TIME_FIELD_YEAR || flash_timer > FLASH_TIMER_ON){
		snprintf(buffer, sizeof(buffer), "%04d", tm.tm_year + 1900);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 0, 0);
	}

	display_buffer->write_char('-', font_5x8, scroll_value + 24, 0);

	//Write the month
	if (flash_field != TIME_FIELD_MONTH || flash_timer > FLASH_TIMER_ON){
		snprintf(buffer, sizeof(buffer), "%02d", tm.tm_mon + 1);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 28, 0);
	}

	display_buffer->write_char('-', font_5x8, scroll_value + 40, 0);

	//Write the day of month
	if (flash_field != TIME_FIELD_DAY_OF_MONTH || flash_timer > FLASH_TIMER_ON){
		snprintf(buffer, sizeof(buffer), "%02d", tm.tm_mday);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 44, 0);
	}

	//Write the day of week
	display_buffer->write_char((char) (tm.tm_wday + 0x41), font_icon, scroll_value + 58, 2);		//In the icon font, Sunday is 'A' (0x41), Monday is 'B', etc.

	if (flash_timer & 0x01){
		if (flash_field == NO_FLASH){		//If we are not flashing, we scroll the entire width
			scroll_value += scroll_direction;
			if (scroll_value < (58 + 7 - 32) * -1 - 4){
				scroll_value = (58 + 7 - 32) * -1 - 4;
				scroll_direction = 1;
			}
			else if (scroll_value > 2){
				scroll_value = 0;
				scroll_direction = -1;
			}
			else if ((scroll_value == (58 + 7 - 32) * -1 - 4) || (scroll_value == 4)){		//The 58 is the position of the day of week; the 7 is the width of the day of week; the 32 is the width of the display.  The 4 (in both comparisons) is to give a bit more room before bouncing.
				scroll_direction *= -1;
			}
		}
		else if (flash_field == TIME_FIELD_YEAR){
			scroll_value = 0;
		}
		else if (flash_field == TIME_FIELD_MONTH){
			if (scroll_value > -20){
				scroll_value--;
			}
		}
		else if (flash_field == TIME_FIELD_DAY_OF_MONTH){
			if (scroll_value > -36){
				scroll_value--;
			}
		}
	}
}
