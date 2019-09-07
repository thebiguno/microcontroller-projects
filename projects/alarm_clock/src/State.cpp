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

	//serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "State: %d (%d)\n\r", state, millis));

	if (mode == MODE_TIME){
		if (button.longPressEvent()){
			mode = MODE_MENU;
			menu_item = 0;
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
	else if (mode == MODE_MENU){
		if (button.longPressEvent()){
			mode = MODE_TIME;
			serial.write("Return to Time\n\r");
		}
		else if (button.releaseEvent()){
			mode = MODE_EDIT;
			edit_item = 0;
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
	else if (mode == MODE_EDIT){
		//Long press from edit commits and returns to main screen
		if (button.longPressEvent()){
			mode = MODE_TIME;
			//serial.write("Return to Time\n\r");
		}

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

			if (button.releaseEvent()){
				edit_item = (edit_item + 1) % 12;	//Here edit_item goes from 0 to 13.  0 - 1 map to TIME_FIELD_HOURS and TIME_FIELD_MINUTES.  2 - 8 map to the bit mask for enabled days.  9 and 10 are a time in minutes for lamp and music ramp-up.  11 is the index for music playback.
			}
			else if (encoder_movement != 0){
				if (edit_item < 2){
					alarm[alarm_index].time = time_add(alarm[alarm_index].time, edit_item + 3, encoder_movement, 0);
				}
				else if (edit_item < 9){
					alarm[alarm_index].enabled ^= _BV(edit_item - 2);
				}
				else if (edit_item == 9){
					alarm[alarm_index].lamp_speed = (alarm[alarm_index].lamp_speed + encoder_movement) % 100;
				}
				else if (edit_item == 10){
					alarm[alarm_index].music_speed = (alarm[alarm_index].music_speed + encoder_movement) % 100;
				}
				else if (edit_item == 11){
					alarm[alarm_index].music_index = (alarm[alarm_index].music_index + encoder_movement) % 100;
				}
			}
		}
		else if (menu_item == MENU_SET_TIME){
			dc_time_t time = get_time();

			if (button.releaseEvent()){
				edit_item = (edit_item + 1) % 7;	//Here edit_item goes from 0 to 6.  0 - 5 map to TIME_FIELD_X; 6 is 12 / 24 hour
			}
			else if (encoder_movement != 0){
				if (edit_item == 0x06){
					calendar.setTime(time_set_mode(time, TIME_MODE_24));
				}
				else {
					calendar.setTime(time_add(time, edit_item, encoder_movement, 0));
				}
			}
		}
	}
}

alarm_t State::get_alarm(uint8_t index){
	if (index >= ALARM_COUNT){
		index = ALARM_COUNT - 1;
	}
	return alarm[index];
}

void State::set_alarm(uint8_t index, alarm_t alarm){
	if (index >= ALARM_COUNT){
		index = ALARM_COUNT - 1;
	}
	this->alarm[index] = alarm;
}

uint8_t State::get_mode(){
	return this->mode;
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
