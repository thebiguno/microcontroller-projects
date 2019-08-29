#include "State.h"
#include "Display.h"
#include "lib/bootloader/bootloader.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

//The set point values to use for voltage calibration, in mV
static int16_t voltage_calibration_values[CALIBRATION_COUNT] = {0, 1800, 2500, 3300, 5000, 8000, 10000, 12500};
//The set point values to use for current calibration, in mA
static int16_t current_calibration_values[CALIBRATION_COUNT] = {5, 10, 25, 50, 100, 500, 1000, 1500};

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

//The UI state machine.  Takes current state + input and moves between states.
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
		// else if (held & BUTTON_2){
		// 	display.force_reset();
		// 	this->state = STATE_MENU;
		// }
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
		else if (held & BUTTON_2){
			display.force_reset();
			this->state = STATE_MENU;
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
				if (channel->get_voltage_limit() > 0){
					channel->set_voltage_setpoint(channel->get_voltage_setpoint() + this->calculate_delta_v(encoder1_movement));
				}
				else {
					channel->set_voltage_setpoint(channel->get_voltage_setpoint() - this->calculate_delta_v(encoder1_movement));
				}
			}
			else if (encoder2_movement){
				if (channel->get_current_limit() > 0){
					channel->set_current_setpoint(channel->get_current_setpoint() + this->calculate_delta_a(encoder2_movement));
				}
				else {
					channel->set_current_setpoint(channel->get_current_setpoint() - this->calculate_delta_a(encoder2_movement));
				}
			}
		}
	}
	else if (this->state == STATE_MENU){
		if (held & BUTTON_1){	//Hold button 1 to go back
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
		}
		else if (released & BUTTON_1){	//Press button 1 to descend into menu
			display.force_reset();
			if (this->get_scroll_menu() == MENU_CALIBRATE_VOLTAGE){
				this->state = STATE_CALIBRATE_VOLTAGE;
				this->calibrate_index = 0;
				Channel* channel = &channels[this->scroll_channel];
				channel->set_voltage_setpoint(voltage_calibration_values[0]);
				channel->set_current_setpoint(channel->get_current_limit());	//When calibrating voltage, we turn off current limiting
			}
			else if (this->get_scroll_menu() == MENU_CALIBRATE_CURRENT){
				this->state = STATE_CALIBRATE_CURRENT;
				this->calibrate_index = 0;
				Channel* channel = &channels[this->scroll_channel];
				channel->set_current_setpoint(current_calibration_values[0]);
				channel->set_voltage_setpoint(channel->get_voltage_limit());	//When calibrating current, we turn voltage to max
			}
			else if (this->get_scroll_menu() == MENU_STARTUP_SETPOINT){
				this->state = STATE_STARTUP_SETPOINT;
			}
			else if (this->get_scroll_menu() == MENU_FIRMWARE){
				this->state = STATE_FIRMWARE;
				display.update(*this);
				_delay_ms(500);
				bootloader_jump(BOOTLOADER_ATMEL);
			}
		}
		else if (encoder1_movement < 0){
			this->scroll_menu--;
			if (this->scroll_menu >= MENU_SIZE){
				this->scroll_menu = MENU_SIZE - 1;
			}
		}
		else if (encoder1_movement > 0){
			this->scroll_menu++;
			if (this->scroll_menu >= MENU_SIZE){
				this->scroll_menu = 0;
			}
		}
	}
	else if (this->state == STATE_CALIBRATE_VOLTAGE || this->state == STATE_CALIBRATE_CURRENT){
		Channel* channel = &channels[this->scroll_channel];
		uint8_t v_calib = this->state == STATE_CALIBRATE_VOLTAGE ? 1 : 0;
		//If we are calibrating right now, HOLD LEFT cancels calibration and loads defaults; PRESS RIGHT goes to next calibration value; Encoder changes calibration.
		if (this->calibrate_index < CALIBRATION_COUNT){
			//Modify the calibration value
			if (encoder1_movement){
				calibration_t calibration = v_calib ? channel->get_calibration_voltage(this->calibrate_index) : channel->get_calibration_current(this->calibrate_index);
				calibration.dac += encoder1_movement / 2;
				calibration.adc = v_calib ? channel->get_voltage_actual_raw() : channel->get_current_actual_raw();
				if (v_calib){
					channel->set_calibration_voltage(this->calibrate_index, calibration);
					channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index]);
				}
				else {
					channel->set_calibration_current(this->calibrate_index, calibration);
					channel->set_current_setpoint(current_calibration_values[this->calibrate_index]);
				}
			}
			//Cancel calibration, and load last saved calibration values, and reset set points to startup values
			else if (held & BUTTON_1){
				display.force_reset();
				channel->load_calibration();
				channel->set_voltage_setpoint(channel->get_voltage_startup());
				channel->set_current_setpoint(channel->get_current_startup());
				this->state = STATE_MENU;
			}
			//Go back
			else if (released & BUTTON_1){
				display.force_reset();
				this->calibrate_index--;
				if (this->calibrate_index > CALIBRATION_COUNT){	//unsigned int wraps to MAX
					this->calibrate_index = 0;
					if (v_calib){
						channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index]);
					}
					else {
						channel->set_current_setpoint(current_calibration_values[this->calibrate_index]);
					}
				}
			}
			//Go forward
			else if (released & BUTTON_2){
				display.force_reset();
				this->calibrate_index++;
				if (this->calibrate_index < CALIBRATION_COUNT){
					if (v_calib){
						channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index]);
					}
					else {
						channel->set_current_setpoint(current_calibration_values[this->calibrate_index]);
					}
				}
			}
		}
		//If we have finished calibration, HOLD LEFT goes back to beginning of calibration to allow more editing; HOLD RIGHT commits calibration and returns to menu.
		else {
			if (held & BUTTON_1){
				display.force_reset();
				this->calibrate_index = 0;
				if (v_calib){
					channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index]);
				}
				else {
					channel->set_current_setpoint(current_calibration_values[this->calibrate_index]);
				}
			}
			else if (held & BUTTON_2){
				display.force_reset();
				channel->save_calibration();
				channel->set_voltage_setpoint(channel->get_voltage_startup());
				channel->set_current_setpoint(channel->get_current_startup());
				this->state = STATE_MENU;
			}
		}
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

uint8_t State::get_calibrate_index(){
	return this->calibrate_index;
}
