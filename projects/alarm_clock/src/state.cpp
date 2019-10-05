#include "state.h"

using namespace digitalcave;

#include <stdio.h>
#include <SerialUSB.h>
SerialUSB serialUSB;
char buffer[64];

static I2CAVR* i2c;
static DS3231* calendar;
static ButtonAVR* lampButton;
static ButtonAVR* musicButton;

//Current time
static time_t currentTime;
static tm_t tm;

//Alarm stuff
static alarm_t alarm[ALARM_COUNT];			//The actual alarms
static uint8_t alarm_triggered = 0;			//_BV(alarm_index) is set when alarm[alarm_index] is triggered.  If we make any changes to the light, this is reset to 0.  When it is non-zero, we incrememnt light / music gradually.
static double light_brightness = 0;			//Keep track of light brightness...

//General stuff
static uint8_t display_brightness = 0;		//The brightness for the LED matrix.  0 - 15.

//Stuff for menus
static uint8_t mode = 0;					//Main modes.  TIME, MENU, EDIT
static int8_t menu_item = 0;				//From 0 to MENU_COUNT - 1.  The currently selected menu item.
static int8_t edit_item = 0;				//Functionality depends on edit item.  Stuff like setting times and alarms.

static time_t last_input_time = 0;			//Last time that a button was pressed / turned
static time_t lamp_turned_on_time = 0;		//Time that the lamp was turned on
static time_t music_turned_on_time = 0;		//Time that the music was started
static time_t last_alarm_trigger_time = 0;	//Time that the alarm was triggered

static void loopValue(int8_t* value, uint8_t min, uint8_t max){
	if (*value < min){
		*value = max;
	}
	else if (*value > max){
		*value = min;
	}
}

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
	state_get_time(&currentTime, &tm);
	srandom(currentTime);
}

//The UI state machine.  Takes current state + input and moves between states.
void state_poll(){
	music_poll();

	state_get_time(&currentTime, &tm);

#ifdef DEBUG
	//serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "millis: %d\n\r", millis));
#endif

	lampButton->sample(timer_millis());
	musicButton->sample(timer_millis());
	int8_t lamp_encoder_movement = encoder_get_movement_1();
	int8_t music_encoder_movement = encoder_get_movement_2();

	for (uint8_t i = 0; i < ALARM_COUNT; i++){
		alarm_t a = alarm[i];

		//Trigger alarm if time matches
		if (a.enabled & _BV(tm.tm_wday)
				&& (alarm_triggered & _BV(i)) == 0x00		//Don't trigger multiple times for the same alarm
				&& a.hour == tm.tm_hour
				&& a.minute == tm.tm_min
				&& 0 == tm.tm_sec){
			alarm_triggered |= _BV(i);
			last_alarm_trigger_time = currentTime;
			light_set(0);
			light_on();
			music_start();
#ifdef DEBUG
//			serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Alarm %d Triggered\n\r", i));
#endif
		}

		//Increment the brightness by the alarm ramp-up values
		if (alarm_triggered & _BV(i)){

			light_brightness = fmin((double) (currentTime - last_alarm_trigger_time) / 60 / a.lamp_speed, 1);			//Range 0 .. 1 over lamp_speed minutes
			display_brightness = 15;
			uint8_t volume = fmax((fmin(1, (double) (currentTime - last_alarm_trigger_time) / 60 / a.music_speed) * a.music_volume), 1);	//Range 0 .. 1 over music_speed minutes, then multiply by music_volume and add one (so that you always start at volume 1)
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
				lamp_turned_on_time = currentTime;
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
			music_toggle();
			if (music_is_playing()){
				edit_item = EDIT_TIME_MUSIC;
				music_turned_on_time = currentTime;
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
			loopValue(&menu_item, 0, MENU_SIZE - 1);
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
				loopValue(&edit_item, 0, 11);
			}
			//Music encoder increments / decrements fields
			else if (music_encoder_movement != 0){
				if (edit_item == 0){
					int8_t h = alarm[alarm_index].hour + music_encoder_movement;
					loopValue(&h, 0, 23);
					alarm[alarm_index].hour = h;
				}
				else if (edit_item == 1){
					int8_t m = alarm[alarm_index].minute + music_encoder_movement;
					loopValue(&m, 0, 59);
					alarm[alarm_index].minute = m;
				}
				else if (edit_item < 9){
					alarm[alarm_index].enabled ^= _BV(edit_item - 2);
				}
				else if (edit_item == 9){
					int8_t s = alarm[alarm_index].lamp_speed + music_encoder_movement;
					loopValue(&s, 0, 30);
					alarm[alarm_index].lamp_speed = s;
				}
				else if (edit_item == 10){
					int8_t s = alarm[alarm_index].music_speed + music_encoder_movement;
					loopValue(&s, 0, 30);
					alarm[alarm_index].music_speed = s;
				}
				else if (edit_item == 11){
					int8_t v = alarm[alarm_index].music_volume + music_encoder_movement;
					loopValue(&v, 0, 30);
					alarm[alarm_index].music_volume = v;
					music_set_volume(v);
				}
			}
		}
		else if (menu_item == MENU_SET_TIME){
			//Lamp encoder changes fields
			if (lamp_encoder_movement != 0){
				edit_item += lamp_encoder_movement;	//Here edit_item goes from 0 to 4: TIME_FIELD_X
				loopValue(&edit_item, 0, 4);
			}
			//Music encoder increments / decrements fields
			else if (music_encoder_movement != 0){

				serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Before: %d-%d-%d %d:%d:%d\n\r", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec));

				if (edit_item == 0){		//Year
					tm.tm_year += music_encoder_movement;
				}
				else if (edit_item == 1){	//Month
					tm.tm_mon += music_encoder_movement;
				}
				else if (edit_item == 2){	//Day
					tm.tm_mday += music_encoder_movement;
				}
				else if (edit_item == 3){	//Hour
					tm.tm_hour += music_encoder_movement;
				}
				else if (edit_item == 4){	//Minute
					tm.tm_sec = 0;		//Reset seconds to zero when changing minutes
					tm.tm_min += music_encoder_movement;
				}

				//We convert to time_t and back to make sure all the fields are valid
				currentTime = mktime(&tm);
				localtime_r(&currentTime, &tm);
				calendar->set(&tm);

				serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "After:  %d-%d-%d %d:%d:%d\n\r", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec));
			}
		}
		else if (menu_item == MENU_DFU){
			if (lampButton->releaseEvent()){
				bootloader_jump(BOOTLOADER_ATMEL);
			}
		}
	}

	//Handle timeouts - fade to dimmer display, and go back to time after certain timeouts depending on mode
	if (lamp_encoder_movement || music_encoder_movement || lampButton->getState() || musicButton->getState()){	//buttons press / turn
		serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "lamp: %d, music: %d, b1: %d, b2: %d\n\r", lamp_encoder_movement, music_encoder_movement, lampButton->getState(), musicButton->getState()));
		last_input_time = currentTime;
	}

	uint32_t seconds_since_last_input = currentTime - last_input_time;

	//Turn off music after 2 hours
	if (music_is_playing() && seconds_since_last_input > (60 * 60 * 2)){
		music_stop();
	}

	//Go back to time after timeouts
	if (mode == MODE_TIME && seconds_since_last_input > 15){
		mode = MODE_TIME;		//Go back to time after 15 seconds without input in time mode
		edit_item = 0;
	}
	else if (mode == MODE_MENU && seconds_since_last_input > 30){
		mode = MODE_TIME;		//Go back to time after 30 seconds without input in menu mode
		edit_item = 0;
	}
	else if (mode == MODE_EDIT && seconds_since_last_input > 90){
		mode = MODE_TIME;		//Go back to time after 90 seconds without input in edit mode
		edit_item = 0;
	}

	//Determine the LED matrix brightness
	if (seconds_since_last_input < 5){
		display_brightness = 15;
	}
	else if (seconds_since_last_input < 20){
		//Fade out to low brightness when not touching anything for a while.  The min brightness is 5 (if light is on), 3 (if daytime), or 0 (otherwise).
		display_brightness = fmax(20 - seconds_since_last_input, light_state() ? 5 : ((tm.tm_hour >= 8 && tm.tm_hour <= 20) ? 3 : 0));
	}
	else {
		display_brightness = light_state() ? 5 : (tm.tm_hour >= 8 && tm.tm_hour <= 20 ? 3 : 0);
	}
	display_brightness = 0;

	serialUSB.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "time: %lu, last: %d, bright: %d\n\r", currentTime, seconds_since_last_input, display_brightness));

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

void state_get_time(time_t* time, tm_t* tm){
	calendar->get(tm);
	*time = mktime(tm);
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
