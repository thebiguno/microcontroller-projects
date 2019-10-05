#include "state.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serialUSB;
char buffer[64];
#endif

static I2CAVR* i2c;
static DS3231* calendar;
static ButtonAVR* lampButton;
static ButtonAVR* musicButton;

//Alarm stuff
static alarm_t alarm[ALARM_COUNT];		//The actual alarms
static uint8_t alarm_triggered = 0;		//_BV(alarm_index) is set when alarm[alarm_index] is triggered.  If we make any changes to the light, this is reset to 0.  When it is non-zero, we incrememnt light / music gradually.
static double light_brightness = 0;		//Keep track of light brightness...

//General stuff
static uint8_t display_brightness = 0;		//The brightness for the LED matrix.  0 - 15.

//Stuff for menus
static uint8_t mode = 0;					//Main modes.  TIME, MENU, EDIT
static int8_t menu_item = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.
static int8_t edit_item = 0;	//Functionality depends on edit item.  Stuff like setting times and alarms.


void state_init(){
	i2c = new I2CAVR();
	calendar = new DS3231(i2c);
	lampButton = new ButtonAVR(&PORTC, PORTC6, 30, 25, 800, 500);
	musicButton = new ButtonAVR(&PORTC, PORTC7, 30, 25, 800, 500);

	timer_init();
	light_init();
	encoder_init();
	music_init();

	eeprom_read_block(alarm, EEPROM_CALIBRATION_OFFSET, sizeof(alarm));

	//Randomize according to the current time.  We use random() during track selection playback
	dc_time_t now = state_get_time();
	srandom((uint32_t) now.year * now.month + now.day_of_week * now.day_of_month + now.hour * now.minute + now.second);
}

//The UI state machine.  Takes current state + input and moves between states.
void state_poll(){
	music_poll();

	dc_time_t time = state_get_time();
	uint32_t millis = timer_millis();

#ifdef DEBUG
	//serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "millis: %d\n\r", millis));
#endif

	lampButton->sample(millis);
	musicButton->sample(millis);
	int8_t lamp_encoder_movement = encoder_get_movement_1();
	int8_t music_encoder_movement = encoder_get_movement_2();

	for (uint8_t i = 0; i < ALARM_COUNT; i++){
		alarm_t a = alarm[i];

		//Trigger alarm if time matches
		if (a.enabled & _BV(time_get_day_of_week(time))
				&& (alarm_triggered & _BV(i)) == 0x00		//Don't trigger multiple times for the same alarm
				&& a.time.mode == time.mode
				&& a.time.hour == time.hour
				&& a.time.minute == time.minute
				&& time.second == 0){
			alarm_triggered |= _BV(i);
			timer_init();		//Reset our timer; we will use this to track how long the light has been on
			millis = 0;
			light_brightness = 0;
			light_set(0);
			light_on();
			music_start();
#ifdef DEBUG
//			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Alarm %d Triggered\n\r", i));
#endif
		}

		//Increment the brightness by the alarm ramp-up values
		if (alarm_triggered & _BV(i)){
			light_brightness = fmin((double) timer_millis() / 60 / 1000 / a.lamp_speed, 1);			//Range 0 .. 1 over lamp_speed minutes
			display_brightness = 15;
			uint8_t volume = fmax((fmin(1, (double) timer_millis() / 60 / 1000 / a.music_speed) * a.music_volume), 1);	//Range 0 .. 1 over music_speed minutes, then multiply by music_volume and add one (so that you always start at volume 1)
			if (volume != music_get_volume()){
				music_set_volume(volume);
			}
		}
	}

	if (light_state()){
		light_set(light_brightness);
	}

	if (mode == MODE_TIME){
		if (lampButton->longPressEvent()){
			mode = MODE_MENU;
			menu_item = 0;
		}
		else if (lampButton->releaseEvent()){
			alarm_triggered = 0x00;
			light_toggle();
			if (light_state()){
				edit_item = EDIT_TIME_LAMP;
			}
			else {
				edit_item = EDIT_TIME_TIME;
			}
		}
		else if (light_state() && lamp_encoder_movement != 0){
			edit_item = EDIT_TIME_LAMP;
			light_brightness += (double) lamp_encoder_movement / 100;
			if (light_brightness < 0) {
				light_brightness = 0;
			}
			else if (light_brightness > 1){
				light_brightness = 1;
			}
		}
		else if (musicButton->releaseEvent()){
			alarm_triggered = 0x00;
			timer_init();		//Reset our timer; we will use this to track how long the music has been playing
			music_toggle();
			if (music_is_playing()){
				edit_item = EDIT_TIME_MUSIC;
			}
			else {
				edit_item = EDIT_TIME_TIME;
			}
		}
		else if (music_encoder_movement != 0 && music_is_playing()){
			edit_item = EDIT_TIME_MUSIC;
			music_set_volume(music_get_volume() + music_encoder_movement);
		}
		else if (lamp_encoder_movement != 0){
			if (edit_item == EDIT_TIME_TIME){
				edit_item = EDIT_TIME_DATE;
			}
			else {
				edit_item = EDIT_TIME_TIME;
			}
		}
	}
	else if (mode == MODE_MENU){
		if (lampButton->longPressEvent()){
			mode = MODE_TIME;
			edit_item = 0;
		}
		else if (lampButton->releaseEvent()){
			mode = MODE_EDIT;
			edit_item = 0;
		}
		else if (lamp_encoder_movement != 0){
			menu_item += lamp_encoder_movement;
			if (menu_item < 0){
				menu_item = MENU_SIZE - 1;
			}
			else if (menu_item >= MENU_SIZE){
				menu_item = 0;
			}
		}
	}
	else if (mode == MODE_EDIT){
		//Long press from lamp commits and returns to main screen
		if (lampButton->longPressEvent()){
			mode = MODE_TIME;
			edit_item = 0;

			eeprom_update_block(alarm, EEPROM_CALIBRATION_OFFSET, sizeof(alarm));
		}
		else if (menu_item >= MENU_SET_ALARM_1 && menu_item <= MENU_SET_ALARM_3){
			//Find alarm index
			uint8_t alarm_index = menu_item;

			//Lamp encoder changes fields
			if (lamp_encoder_movement != 0){
				edit_item += lamp_encoder_movement;	//Here edit_item goes from 0 to 11.  0 - 1 map to TIME_FIELD_HOURS and TIME_FIELD_MINUTES.  2 - 8 map to the bit mask for enabled days.  9 is a time in minutes for lamp ramp-up.
				if (edit_item < 0){
					edit_item = 11;
				}
				else if (edit_item > 11){
					edit_item = 0;
				}
			}
			//Music encoder increments / decrements fields
			else if (music_encoder_movement != 0){
				if (edit_item < 2){
					alarm[alarm_index].time = time_add(alarm[alarm_index].time, edit_item + 3, music_encoder_movement, 0);
				}
				else if (edit_item < 9){
					alarm[alarm_index].enabled ^= _BV(edit_item - 2);
				}
				else if (edit_item == 9){
					int8_t s = alarm[alarm_index].lamp_speed;
					s += music_encoder_movement;
					if (s < 0){
						s = 0;
					}
					else if (2 > 30){
						s = 30;
					}
					alarm[alarm_index].lamp_speed = s;
				}
				else if (edit_item == 10){
					int8_t s = alarm[alarm_index].music_speed;
					s += music_encoder_movement;
					if (s < 0){
						s = 0;
					}
					else if (2 > 30){
						s = 30;
					}
					alarm[alarm_index].music_speed = s;
				}
				else if (edit_item == 11){
					int8_t v = alarm[alarm_index].music_volume;
					v += music_encoder_movement;
					if (v < 0){
						v = 0;
					}
					else if (v > 30){
						v = 30;
					}
					alarm[alarm_index].music_volume = v;
					music_set_volume(v);
				}
			}
		}
		else if (menu_item == MENU_SET_TIME){
			//Lamp encoder changes fields
			if (lamp_encoder_movement != 0){
				edit_item = (edit_item + lamp_encoder_movement) % 6;	//Here edit_item goes from 0 to 6.  0 - 4 map to TIME_FIELD_X; 5 is 12 / 24 hour
			}
			//Music encoder increments / decrements fields
			else if (music_encoder_movement != 0){
				if (edit_item == 0x05){
					uint8_t mode = time.mode == TIME_MODE_24 ? TIME_MODE_12 : TIME_MODE_24;
					calendar->setTime(time_set_mode(time, mode));
					for (uint8_t i = 0; i < ALARM_COUNT; i++){
						alarm[i].time = time_set_mode(alarm[i].time, mode);
					}
				}
				else {
					if (edit_item == TIME_FIELD_MINUTE){
						time.second = 0;		//Reset seconds to zero when changing minutes
					}
					calendar->setTime(time_add(time, edit_item, music_encoder_movement, 0));
				}
			}
		}
		else if (menu_item == MENU_DFU){
			if (lampButton->releaseEvent()){
				bootloader_jump(BOOTLOADER_ATMEL);
			}
		}
	}

	//Handle timeouts - fade to dimmer display, and go back to time after certain timeouts depending on mode
	static uint32_t last_change = 0;
	if (lamp_encoder_movement || music_encoder_movement || lampButton->getState() || musicButton->getState()){	//buttons press / turn
		last_change = millis;
	}

	//Turn off music after 2 hours
	if (music_is_playing() && millis > 7200000){
		music_stop();
	}

	//Go back to time after timeouts
	if (last_change > millis){	//Handle timer overflow
		last_change = millis;
	}
	else if (mode == MODE_TIME && (last_change + 15000) < millis){
		mode = MODE_TIME;		//Go back to time after 15 seconds without input in time mode
		edit_item = 0;
	}
	else if (mode == MODE_MENU && (last_change + 30000) < millis){
		mode = MODE_TIME;		//Go back to time after 30 seconds without input in menu mode
		edit_item = 0;
	}
	else if (mode == MODE_EDIT && (last_change + 90000) < millis){
		mode = MODE_TIME;		//Go back to time after 90 seconds without input in edit mode
		edit_item = 0;
	}

	//Fade display brightness depending on user input
	if ((last_change + 5000) > millis){
		if (display_brightness < 15){
			display_brightness++;		//Quickly fade up to high brightness when touching input
		}
		else {
			display_brightness = 15;
		}
	}
	else if ((last_change + 30000) > millis){
		display_brightness = ((last_change + 30000) - millis) / 1000;		//Fade out to low brightness when not touching anything for a while
	}
	else {
		display_brightness = 0;
	}

	dc_time brightness_cutoff_early = {2000, 1, 1, 0, 8, 0, 0, TIME_MODE_24};	//8 AM
	dc_time brightness_cutoff_late = {2000, 1, 1, 0, 20, 0, 0, TIME_MODE_24};	//8 PM
	if (display_brightness < 5 && time_after(time, brightness_cutoff_early) && time_before(time, brightness_cutoff_late)){
		display_brightness = 5;
	}
	else if (display_brightness < 10 && light_state()){
		display_brightness = 10;
	}
}

alarm_t state_get_alarm(uint8_t index){
	if (index >= ALARM_COUNT){
		index = ALARM_COUNT - 1;
	}
	return alarm[index];
}

uint8_t state_get_mode(){
	return mode;
}

dc_time_t state_get_time(){
	return calendar->getTime();
}
uint8_t state_get_menu_item(){
	return menu_item;
}

uint8_t state_get_edit_item(){
	return edit_item;
}

uint8_t state_get_display_brightness(){
	return display_brightness;
}

double state_get_lamp_brightness(){
	return light_brightness;
}
