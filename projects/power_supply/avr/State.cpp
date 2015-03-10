#include "State.h"
#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

State::State(){

}

void State::poll(){
	int8_t encoder1_movement = encoders.get_encoder1_movement();
	int8_t encoder2_movement = encoders.get_encoder2_movement();
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
		else if (encoder1_movement > 0){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder1_movement < 0){
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
		else if (encoder1_movement > 0){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (encoder1_movement < 0){
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
		else if (held & BUTTON_2){
			display.force_refresh();
			this->scroll_channel++;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = 0;
			}
		}
		else if (encoder1_movement || encoder2_movement){
			display.force_refresh();
			Channel* channel = &channels[this->scroll_channel];

			//Modify the value
			if (encoder1_movement) channel->adjust_setpoint(SELECTOR_VOLTAGE, 100 * encoder1_movement);
			else if (encoder2_movement) channel->adjust_setpoint(SELECTOR_CURRENT, 10 * encoder2_movement);
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
