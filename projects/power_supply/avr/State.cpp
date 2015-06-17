#include "State.h"
#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

State::State(){

}

int16_t State::calculate_delta_a(int8_t encoder_movement){
	if (encoder_movement > 25) return 100;
	else if (encoder_movement > 8) return 10;
	else if (encoder_movement > 0) return 1;
	else if (encoder_movement <= -25) return -100;
	else if (encoder_movement < -8) return -10;
	else if (encoder_movement < 0) return -1;
	else return 0;
}

int16_t State::calculate_delta_v(int8_t encoder_movement){
	if (encoder_movement > 25) return 1000;
	else if (encoder_movement > 8) return 100;
	else if (encoder_movement > 0) return 10;
	else if (encoder_movement < -25) return -1000;
	else if (encoder_movement < -8) return -100;
	else if (encoder_movement < 0) return -10;
	else return 0;
}

void State::poll(){
	int8_t encoder1_movement = encoders.get_encoder1_movement();
	int8_t encoder2_movement = encoders.get_encoder2_movement();
	uint8_t released = encoders.get_released();
	uint8_t held = encoders.get_held();
	
	if (this->state == STATE_LOCKED){
		if (held & BUTTON_1){
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
		}
		else if (held & BUTTON_2){
			display.force_reset();
			this->state = STATE_MENU;
		}
		else if (encoder1_movement > 0){
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder1_movement < 0){
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
	}
	else if (this->state == STATE_EDIT_ITEM){
		if (held & BUTTON_1){
			display.force_reset();
			this->state = STATE_LOCKED;
		}
		else if (released & BUTTON_1){
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (released & BUTTON_2){
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
		else if (encoder1_movement || encoder2_movement){
			Channel* channel = &channels[this->scroll_channel];

			//Modify the value
			if (encoder1_movement){
				if (channel->get_voltage_limit() > 0) channel->set_voltage_setpoint(channel->get_voltage_setpoint() + this->calculate_delta_v(encoder1_movement));
				else channel->set_voltage_setpoint(channel->get_voltage_setpoint() - this->calculate_delta_v(encoder1_movement));
			}
			else if (encoder2_movement){
				if (channel->get_current_limit() > 0) channel->set_current_setpoint(channel->get_current_setpoint() + this->calculate_delta_a(encoder2_movement));
				else channel->set_current_setpoint(channel->get_current_setpoint() - this->calculate_delta_a(encoder2_movement));
			}
		}
	}
	else if (this->state == STATE_MENU){
		//TODO...
	}
}

uint8_t State::get_state(){
	return this->state;
}

uint8_t State::get_scroll_channel(){
	return this->scroll_channel;
}

uint8_t State::get_scroll_menu(){
	return this->scroll_menu;
}
