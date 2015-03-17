#include "State.h"
#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

State::State(){

}

int16_t State::calculate_delta(int8_t encoder_movement){
	int16_t delta = 0;
	if (encoder_movement > 7) return 1000;
	else if (encoder_movement > 6) return 500;
	else if (encoder_movement > 5) return 250;
	else if (encoder_movement > 4) return 100;
	else if (encoder_movement > 3) return 50;
	else if (encoder_movement > 2) return 25;
	else if (encoder_movement > 1) return 10;
	else if (encoder_movement < -7) return -1000;
	else if (encoder_movement < -6) return -500;
	else if (encoder_movement < -5) return -250;
	else if (encoder_movement < -4) return -100;
	else if (encoder_movement < -3) return -50;
	else if (encoder_movement < -2) return -25;
	else if (encoder_movement < -1) return -10;
	
	
	return delta;
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
				channel->set_voltage_setpoint(channel->get_voltage_setpoint() + this->calculate_delta(encoder1_movement));
			}
			else if (encoder2_movement){
				channel->set_current_setpoint(channel->get_current_setpoint() + this->calculate_delta(encoder2_movement));
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
