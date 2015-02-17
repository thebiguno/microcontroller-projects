#include "State.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];

State::State(){

}

void State::poll(){
	uint8_t encoder_movement = encoders.get_encoder_movement();
	uint8_t released = encoders.get_released();
	uint8_t held = encoders.get_held();
	
	if (this->state == STATE_LOCKED){
		if (held & BUTTON_1){
			this->state = STATE_EDIT;
		}
		else if (held & BUTTON_2){
			this->state = STATE_MENU;
		}
		else if (encoder_movement & ENCODER1_CLOCKWISE){
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE){
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
	}
	else if (this->state == STATE_EDIT){
		if (released & BUTTON_1){
			this->state = STATE_EDIT_ITEM;
		}
		else if (encoder_movement & ENCODER1_CLOCKWISE){
			this->scroll_value++;
			if (this->scroll_value > 1){
				this->scroll_value = 0;
				this->scroll_channel++;
				if (this->scroll_channel >= CHANNEL_COUNT){
					this->scroll_channel = CHANNEL_COUNT - 1;
					this->scroll_value = 1;
				}
			}
		}
		else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE){
			this->scroll_value--;
			if (this->scroll_value > 1){
				this->scroll_value = 1;
				this->scroll_channel--;
				if (this->scroll_channel >= CHANNEL_COUNT){
					this->scroll_channel = 0;
					this->scroll_value = 0;
				}
			}
		}
	}
	else if (this->state == STATE_EDIT_ITEM){
		if (released & BUTTON_1){
			this->state = STATE_EDIT;
		}
		else if (encoder_movement){
			Channel* channel = &channels[this->scroll_value >> 1];
			uint8_t selector = this->scroll_value & 0x01;

			//Modify the value
			if (encoder_movement & ENCODER1_CLOCKWISE) channel->adjust_setpoint(selector, 1);
			else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -1);
			else if (encoder_movement & ENCODER2_CLOCKWISE) channel->adjust_setpoint(selector, 0.01);
			else if (encoder_movement & ENCODER2_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -0.01);
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

uint8_t State::get_scroll_value(){
	return this->scroll_value;
}

uint8_t State::get_scroll_menu(){
	return this->scroll_menu;
}
