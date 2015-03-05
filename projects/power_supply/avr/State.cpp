#include "State.h"
#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

State::State(){

}

void State::poll(){
	uint8_t encoder_movement = encoders.get_encoder_movement();
	uint8_t released = encoders.get_released();
	uint8_t held = encoders.get_held();
	
	if (this->state == STATE_LOCKED){
		if (held & BUTTON_1){
			display.force_reset();
			this->state = STATE_EDIT;
		}
		else if (held & BUTTON_2){
			display.force_reset();
			this->state = STATE_MENU;
		}
		else if (encoder_movement & ENCODER1_CLOCKWISE){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE){
			display.force_refresh();
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
	}
	else if (this->state == STATE_EDIT){
		if (released & BUTTON_1){
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
		}
		else if (held & BUTTON_1){
			display.force_reset();
			this->state = STATE_LOCKED;
		}
		else if (encoder_movement & ENCODER1_CLOCKWISE){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE){
			display.force_refresh();
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
	}
	else if (this->state == STATE_EDIT_ITEM){
		if (released & BUTTON_1){
			display.force_reset();
			this->state = STATE_EDIT;
		}
		else if (held & BUTTON_1){
			display.force_reset();
			this->state = STATE_LOCKED;
		}
		else if (released & BUTTON_2){
			display.force_refresh();
			this->scroll_value ^= 0x01;
		}
		else if (held & BUTTON_2){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
		else if (encoder_movement){
			display.force_refresh();
			Channel* channel = &channels[this->scroll_channel];
			uint8_t selector = this->scroll_value;

			//Modify the value
			if (encoder_movement & ENCODER1_CLOCKWISE) channel->adjust_setpoint(selector, 1000);
			else if (encoder_movement & ENCODER1_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -1000);
			else if (encoder_movement & ENCODER2_CLOCKWISE) channel->adjust_setpoint(selector, 10);
			else if (encoder_movement & ENCODER2_COUNTER_CLOCKWISE) channel->adjust_setpoint(selector, -10);
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
