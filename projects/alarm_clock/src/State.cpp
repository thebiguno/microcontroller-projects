#include "State.h"

using namespace digitalcave;

State::State() :
	i2c(),
	calendar(&i2c),
	encoder(),
	button(&PORTC, PORTC6, 30, 25, 800, 500)
{
	timer_init();
	light_init();
}

//The UI state machine.  Takes current state + input and moves between states.
void State::poll(){
	uint32_t millis = timer_millis();
	button.sample(millis);
	int8_t encoder_movement = encoder.get_encoder_movement();

	if (state == STATE_TIME){
		if (button.longPressEvent()){
			state = STATE_MENU;
		}
		else if (button.releaseEvent()){
			if (light_brightness){
				light_brightness = 0;
				light_off();
			}
			else {
				light_brightness = 512;
				light_on();
			}
		}
		else if (encoder_movement != 0 && light_brightness != 0){
			light_brightness += encoder_movement * 20;
			if (light_brightness < 1) {
				light_brightness = 1;
			}
			else if (light_brightness > 1024){
				light_brightness = 1024;
			}
		}
	}
	else if (state == STATE_MENU){
		if (button.longPressEvent()){
			state = STATE_TIME;
		}
		else if (button.releaseEvent()){
			state = STATE_EDIT;
		}
		else if (encoder_movement != 0){
			if (encoder_movement > 0){
				menu_item++;
				if (menu_item >= MENU_SIZE){
					menu_item = 0;
				}
			}
			else {
				menu_item--;
				if (menu_item >= MENU_SIZE){
					menu_item = MENU_SIZE - 1;
				}
			}
		}
	}
	else if (state == STATE_EDIT){
		if (menu_item == MENU_ALARM_1){
			if (button.longPressEvent()){
				state = STATE_MENU;
			}
			else if (button.releaseEvent()){
				state = STATE_TIME;
			}

		}
		else if (menu_item == MENU_12_24){
			ds3231_time_t time = get_time();
			if (button.longPressEvent() || button.releaseEvent()){
				state = STATE_TIME;
			}
			else if (encoder_movement != 0){
				if (time.mode == DS3231_MODE_24){
					if (time.hour > 12){
						time.hour = time.hour - 12;
						time.mode = DS3231_MODE_PM;
					}
					else {
						time.mode = DS3231_MODE_AM;
					}
				}
				else {
					if (time.mode ==DS3231_MODE_PM){
						time.hour = time.hour + 12;
					}
					time.mode = DS3231_MODE_24;
				}
				calendar.setTime(time);
			}
		}
	}
}

uint8_t State::get_state(){
	return this->state;
}

ds3231_time_t State::get_time(){
	return calendar.getTime();
}
uint8_t State::get_menu_item(){
	return this->menu_item;
}

int16_t State::get_light_brightness(){
	return this->light_brightness;
}
