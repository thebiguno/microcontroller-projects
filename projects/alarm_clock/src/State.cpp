#include "State.h"

using namespace digitalcave;

extern SerialUSB serial;

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
	char buffer[64];

	serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "State: %d (%d)\n\r", state, millis));

	if (state == STATE_TIME){
		if (button.longPressEvent()){
			state = STATE_MENU;
			serial.write("Enter Menu\n\r");
		}
		else if (button.releaseEvent()){
			if (light_brightness){
				light_brightness = 0;
				light_off();
				serial.write("Light Off\n\r");
			}
			else {
				light_brightness = 512;
				light_on();
				serial.write("Light On\n\r");
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
			serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Light Brightness: %d\n\r", light_brightness));
		}
	}
	else if (state == STATE_MENU){
		if (button.longPressEvent()){
			state = STATE_TIME;
			serial.write("Return to Time\n\r");
		}
		else if (button.releaseEvent()){
			state = STATE_EDIT;
			serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Editing Menu Item: %d\n\r", menu_item));
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
		serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Menu Item: %d\n\r", menu_item));
		}
	}
	else if (state == STATE_EDIT){
		if (menu_item == MENU_SET_ALARM_1){
			if (button.longPressEvent()){
				state = STATE_MENU;
				serial.write("Alarm 1 Return to Menu\n\r");
			}
			else if (button.releaseEvent()){
				state = STATE_TIME;
				serial.write("Alarm 1 Return to Time\n\r");
			}

		}
		else if (menu_item == MENU_SET_TIME){
			dc_time_t time = get_time();

			if (button.longPressEvent()){
				state = STATE_TIME;
				serial.write("Set Time Return to Time\n\r");
			}
			else if (button.releaseEvent()){
				edit_item++;
				if (edit_item > 2){
					edit_item = 0;
				}
				serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "Set Time Change Field: %d\n\r", edit_item));
			}
			else if (encoder_movement != 0){
				serial.write("Set Time\n\r");
				time = time_add(time, edit_item + TIME_FIELD_HOUR, encoder_movement, 0);
				calendar.setTime(time);
			}
		}
		else if (menu_item == MENU_12_24){
			dc_time_t time = get_time();
			if (button.longPressEvent() || button.releaseEvent()){
				state = STATE_TIME;
			}
			else if (encoder_movement != 0){
				//Whenever we move the dial, we change from 12 to 24 hour mode.
				time = time_set_mode(time, TIME_MODE_24);
				calendar.setTime(time);
			}
			serial.write("12 / 24 Hour Mode\n\r");
		}
	}
}

uint8_t State::get_state(){
	return this->state;
}

dc_time_t State::get_time(){
	return calendar.getTime();
}
uint8_t State::get_menu_item(){
	return this->menu_item;
}

uint8_t State::get_edit_item(){
	return this->edit_item;
}

int16_t State::get_light_brightness(){
	return this->light_brightness;
}
