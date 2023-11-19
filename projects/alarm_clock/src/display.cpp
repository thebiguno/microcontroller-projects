#include "display.h"

using namespace digitalcave;

static Buffer* display_buffer;

static void display_write_time(time_t time, int8_t flash_field);
static void display_write_date(time_t time, int8_t flash_field);

//This determines how fast things flash
#define FLASH_TIMER_PERIOD	0x7F
#define FLASH_TIMER_PHASE	0x3F
#define NO_FLASH			0xFF
static uint8_t flash_timer = 0;

//This determines how fast things scroll
#define SCROLL_TIMER_PERIOD	0x07
static uint8_t scroll_timer = 0;
static int8_t scroll_value = 0;

//These exist in state.cpp
extern time_t now;
extern tm_t now_tm;

static char buffer[10];

void display_init(){
	display_buffer = new Buffer(32, 8);
	display_buffer->clear();

	max7219_init(4);
}

void display_update(){
	display_buffer->clear();

	flash_timer++;
	if (flash_timer > FLASH_TIMER_PERIOD){
		flash_timer = 0;
	}

	scroll_timer++;
	if (scroll_timer > SCROLL_TIMER_PERIOD){
		scroll_timer = 0;
	}

	config_t config = state_get_config();

	uint8_t mode = state_get_mode();
	uint8_t menu_item = state_get_menu_item();
	uint8_t edit_item = state_get_edit_item();

	if (mode == MODE_TIME){
		if (edit_item == EDIT_TIME_TIME){
			scroll_value = 4;
			display_write_time(now, -1);
		}
		else if (edit_item == EDIT_TIME_LAMP){
			display_buffer->write_char(light_state() ? 'M' : 'N', font_icon, 3, 0);					//Icon M / N is lamp on / off
			snprintf(buffer, sizeof(buffer), "%d", config.lamp_brightness);
			display_buffer->write_string(buffer, font_5x8, config.lamp_brightness < 10 ? 23 : (config.lamp_brightness < 100 ? 17 : 12), 0);
		}
		else if (edit_item == EDIT_TIME_MUSIC){
			display_buffer->write_char(music_is_playing() ? 'K' : 'L', font_icon, 3, 0);						//Icon K / L is music on / off
			snprintf(buffer, sizeof(buffer), "%d", config.volume);
			display_buffer->write_string(buffer, font_5x8, (config.volume < 10 ? 23 : 17), 0);
		}
	}
	else if (mode == MODE_MENU){
		if (menu_item <= MENU_SET_ALARM_3){		//Alarm 1, 2, or 3
			display_buffer->write_string("SET", font_3x5, 2, 2);
			display_buffer->write_char('I', font_icon, 16, 0);			//Alarm icon
			display_buffer->write_char((char) (menu_item + 0x31 - MENU_SET_ALARM_1), font_5x8, 25, 0);
		}
		else if (menu_item == MENU_SET_TIME){
			display_buffer->write_string("SET", font_3x5, 5, 2);
			display_buffer->write_char('J', font_icon, 19, 0);			//Clock icon
		}
		else if (menu_item == MENU_CONFIG){
			display_buffer->write_char('T', font_icon, 0, 0);			//Wrench icon
			display_buffer->write_string("CONFIG", font_3x5, 9, 2);
		}
	}
	else if (mode == MODE_MUSIC_MENU){
		display_buffer->write_char('K', font_icon, 0, 0);			//Music
		display_buffer->write_char('P', font_icon, 9, 0);			//Folders
		snprintf(buffer, sizeof(buffer), "%02d", config.source);
		display_buffer->write_string(buffer, font_5x8, 21, 0);
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

			if (edit_item == 0){
				display_buffer->write_char('I', font_icon, 3, 0);			//0 is alarm icon
				display_buffer->write_char((char) (menu_item + 0x31 - MENU_SET_ALARM_1), font_5x8, 12, 0);
				display_buffer->write_char(alarm.enabled & _BV(7) ? 'R' : 'S', font_icon, 21, 0);			//Checkmark or Cross
			}
			else if (edit_item == 1 || edit_item == 2){
				tm_t now_tm;
				localtime_r(&now, &now_tm);
				now_tm.tm_hour = alarm.hour;
				now_tm.tm_min = alarm.minute;
				display_write_time(mktime(&now_tm), edit_item - 1);
			}
			else if (edit_item == 3){										//Lamp brightness
				display_buffer->write_char('M', font_icon, 2, 0);			//Brightness
				snprintf(buffer, sizeof(buffer), "%d", alarm.lamp_brightness);
				display_buffer->write_string(buffer, font_5x8, alarm.lamp_brightness < 10 ? 26 : (alarm.lamp_brightness < 100 ? 20 : 14), 0);
			}
			else if (edit_item == 4){										//Lamp rampup time
				display_buffer->write_char('M', font_icon, 2, 0);			//Brightness
				display_buffer->write_char('J', font_icon, 11, 0);			//Clock
				snprintf(buffer, sizeof(buffer), "%d", alarm.lamp_speed);
				display_buffer->write_string(buffer, font_5x8, (alarm.lamp_speed < 10 ? 26 : 20), 0);
			}
			else if (edit_item == 5){										//Music volume
				display_buffer->write_char('K', font_icon, 2, 0);			//Music
				snprintf(buffer, sizeof(buffer), "%d", alarm.music_volume);
				display_buffer->write_string(buffer, font_5x8, (alarm.music_volume < 10 ? 26 : 20), 0);
			}
			else if (edit_item == 6){										//Music rampup time
				display_buffer->write_char('K', font_icon, 2, 0);			//Music
				display_buffer->write_char('J', font_icon, 11, 0);			//Clock
				snprintf(buffer, sizeof(buffer), "%d", alarm.music_speed);
				display_buffer->write_string(buffer, font_5x8, (alarm.music_speed < 10 ? 26 : 20), 0);
			}
			else if (edit_item == 7){										//Music folder number
				display_buffer->write_char('K', font_icon, 2, 0);			//Icon 2 is music
				display_buffer->write_char('P', font_icon, 11, 0);			//Icon 5 is folders
				snprintf(buffer, sizeof(buffer), "%02d", alarm.music_source);
				display_buffer->write_string(buffer, font_5x8, 20, 0);
			}
			else {
				int8_t day_index = edit_item - 8;
				if (scroll_timer == 0 && scroll_value != (day_index * -7) + 14){
					if (scroll_value < (day_index * -7) + 14) scroll_value++;
					else scroll_value--;
				}

				for (uint8_t i = 0; i < 7; i++){
					if (alarm.enabled & _BV(i)){
						//Draw the underline for days that are enabled
						display_buffer->write_char('H', font_icon, (i * 7) + scroll_value, 0);
					}
					if (flash_timer > FLASH_TIMER_PHASE || day_index != i){
						//Write the days
						display_buffer->write_char((char) (i + 0x41), font_icon, (i * 7) + scroll_value, 0);		//In the Icon font, we define Sunday as 'A' (0x41), Monday as 'B', etc.  So (i + 0x41) returns the day of the week.
					}
				}
			}
		}
		else if (menu_item == MENU_SET_TIME){
			if (edit_item <= 2){
				display_write_date(now, edit_item);
			}
			else {
				display_write_time(now, edit_item - 3);
			}
		}
		else if (menu_item == MENU_CONFIG){
			if (edit_item <= 7){										//Config file count in each folder
				display_buffer->write_char('Q', font_icon, 1, 0);			//Files
				display_buffer->write_char((char) (edit_item + 0x31), font_5x8, 11, 0);
				snprintf(buffer, sizeof(buffer), "%d", config.music_count[edit_item]);
				display_buffer->write_string(buffer,
					(config.music_count[edit_item] < 100 ? font_5x8 : font_3x5),	//Font - smaller for >= 100
					(config.music_count[edit_item] < 10 ? 26 : 20),					//Horizontal Position - right justify for < 10
					(config.music_count[edit_item] < 100 ? 0 : 2));					//Vertical Postion - move down for >= 100
			}
			else if (edit_item == 8){
				display_buffer->write_string("O", font_icon, 0, 0);			//DFU Upload
				display_buffer->write_string("UPLOAD", font_3x5, 8, 2);
			}
		}
	}

	max7219_write_buffer(display_buffer->get_data());

	max7219_set_brightness(state_get_display_brightness());
	#ifdef DEBUG
	PORTF |= _BV(PORTF4);
	#endif
	max7219_write_command(MAX7219_SHUTDOWN, 0x01);
	delay_us(1);	//This value combined with the next global delay will (somewhat) determine how dim the LED matrix is when "brightness == 0".
	#ifdef DEBUG
	PORTF &= ~_BV(PORTF4);
	#endif

	if (state_get_display_brightness() == 0){
		max7219_write_command(MAX7219_SHUTDOWN, 0x00);
	}

	delay_us(1500);
}


static void display_write_time(time_t time, int8_t flash_field){
	tm_t now_tm;
	localtime_r(&time, &now_tm);
	//
	// strftime(buffer, sizeof(buffer), "%l:%M%p", &now_tm);
	// display_buffer->write_string(buffer, font_5x8, 0, 0);

	//Write the hours
	uint8_t hour = now_tm.tm_hour;
	if (hour == 0) hour = 12;
	else if (hour > 12) hour -= 12;
	if (flash_field != 0 || flash_timer > FLASH_TIMER_PHASE){
		snprintf(buffer, sizeof(buffer), "%d", hour);
		display_buffer->write_string(buffer, font_5x8, (hour >= 10 ? 1 : 5), 0);	//Variable width fonts don't allow for using printf's spacing, so we do it manually
	}

	//Write the colon
	display_buffer->write_string(":", font_5x8, (hour >= 10 ? 13 : 11), 0);

	//Write the minutes
	if (flash_field != 1 || flash_timer > FLASH_TIMER_PHASE){
		snprintf(buffer, sizeof(buffer), "%02d", now_tm.tm_min);
		display_buffer->write_string(buffer, font_5x8, (hour >= 10 ? 15 : 13), 0);
	}

	//Write AM / PM
	display_buffer->write_char((now_tm.tm_hour < 12 ? 'A' : 'P'), font_5x8, (hour >= 10 ? 28 : 26), 0);
}

static void display_write_date(time_t time, int8_t flash_field){
	tm_t now_tm;
	localtime_r(&time, &now_tm);

	//Write the year
	if (flash_field != 0 || flash_timer > FLASH_TIMER_PHASE){
		snprintf(buffer, sizeof(buffer), "%04d", now_tm.tm_year + 1900);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 0, 0);
	}

	display_buffer->write_char('-', font_5x8, scroll_value + 24, 0);

	//Write the month
	if (flash_field != 1 || flash_timer > FLASH_TIMER_PHASE){
		snprintf(buffer, sizeof(buffer), "%02d", now_tm.tm_mon + 1);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 28, 0);
	}

	display_buffer->write_char('-', font_5x8, scroll_value + 40, 0);

	//Write the day of month
	if (flash_field != 2 || flash_timer > FLASH_TIMER_PHASE){
		snprintf(buffer, sizeof(buffer), "%02d", now_tm.tm_mday);
		display_buffer->write_string(buffer, font_5x8, scroll_value + 44, 0);
	}

	//Write the day of week
	display_buffer->write_char((char) (now_tm.tm_wday + 0x41), font_icon, scroll_value + 58, 2);		//In the icon font, Sunday is 'A' (0x41), Monday is 'B', etc.

	if (scroll_timer == 0){
		if (flash_field == -1){		//If we are not flashing, we scroll the entire width
			scroll_value -= 1;
		}
		else if (flash_field == 0){
			if (scroll_value != 1){
				scroll_value += (scroll_value > 1 ? -1 : 1);
			}
		}
		else if (flash_field == 1){
			if (scroll_value != -18){
				scroll_value += (scroll_value > -18 ? -1 : 1);
			}
		}
		else if (flash_field == 2){
			if (scroll_value != -34){
				scroll_value += (scroll_value > -34 ? -1 : 1);
			}
		}
	}
}
