#include "State.h"
#include "Display.h"		//Included here instead of State.h to avoid circular dependency

using namespace digitalcave;

extern ButtonAVR button1;
extern ButtonAVR button2;

extern Channel channels[CHANNEL_COUNT];
extern Display display;

//extern SerialUSB serial;

//The set point values to use for voltage calibration, in mV
static int16_t voltage_calibration_values[CALIBRATION_COUNT] = {0, 1800, 2500, 3300, 5000, 8000, 10000, 12500};
//The set point values to use for current calibration, in mA
static int16_t current_calibration_values[CALIBRATION_COUNT] = {0, 10, 25, 50, 100, 500, 1000, 1500};

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
	uint32_t millis = timer_millis();
	button1.sample(millis);
	button2.sample(millis);
	int8_t encoder1_movement = encoders.get_encoder1_movement();
	int8_t encoder2_movement = encoders.get_encoder2_movement();

	if (this->state == STATE_LOCKED){
		if (button2.getState() == BUTTON_LONG_PRESSED && encoder1_movement > 0){	//Hold down button2 for 1 second, followed by turning button1 clockwise when in lock screen to update firmware
			this->state = STATE_FIRMWARE;
			//Update the display to show directions
			display.update(*this);
			_delay_ms(500);
			bootloader_jump(BOOTLOADER_ATMEL);
		}
		else if (button1.longPressEvent()){
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
		}
		// else if (encoder1_movement > 0){
		// 	this->scroll_channel++;
		// 	if (this->scroll_channel >= CHANNEL_COUNT){
		// 		this->scroll_channel = CHANNEL_COUNT - 1;
		// 	}
		// }
		// else if (encoder1_movement < 0){
		// 	this->scroll_channel--;
		// 	if (this->scroll_channel >= CHANNEL_COUNT){
		// 		this->scroll_channel = 0;
		// 	}
		// }
	}
	else if (this->state == STATE_EDIT_ITEM){
		if (button1.longPressEvent()){
			display.force_reset();
			this->state = STATE_LOCKED;
		}
		else if (button2.longPressEvent()){
			display.force_reset();
			this->state = STATE_MENU;
		}
		else if (button1.releaseEvent()){
			this->scroll_channel--;
			if (this->scroll_channel >= CHANNEL_COUNT){
				this->scroll_channel = CHANNEL_COUNT - 1;
			}
		}
		else if (button2.releaseEvent()){
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
			if (encoder2_movement){
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
		Channel* channel = &channels[this->scroll_channel];
		int8_t negate = channel->get_voltage_limit() < 0 ? -1 : 1;		//We assume that a channel will be negative for both voltage and current.  This should always hold true.
		if (button1.longPressEvent()){	//Hold button 1 to go back
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
		}
		else if (button2.longPressEvent()){	//Hold button 2 to descend into menu
			display.force_reset();
			if (this->get_scroll_menu() == MENU_CALIBRATE_VOLTAGE){
				this->state = STATE_CALIBRATE_VOLTAGE;
				this->calibrate_index = 0;
				channel->set_voltage_setpoint(voltage_calibration_values[0] * negate);
				channel->set_current_setpoint(channel->get_current_limit());	//When calibrating voltage, we turn off current limiting
			}
			else if (this->get_scroll_menu() == MENU_CALIBRATE_CURRENT){
				this->state = STATE_CALIBRATE_CURRENT;
				this->calibrate_index = 0;
				channel->set_current_setpoint(current_calibration_values[0] * negate);
				channel->set_voltage_setpoint(channel->get_voltage_limit());	//When calibrating current, we turn voltage to max
			}
			else if (this->get_scroll_menu() == MENU_STARTUP_SETPOINT){
				this->state = STATE_STARTUP_SETPOINT;
				this->voltage_setpoint = channel->get_voltage_startup();
				this->current_setpoint = channel->get_current_startup();
			}
			// else if (this->get_scroll_menu() == MENU_SET_AREF_SOURCE){
			// 	this->state = STATE_SET_AREF_SOURCE;
			// }
			// else if (this->get_scroll_menu() == MENU_SET_DAC_ADDRESS){
			// 	this->state = STATE_SET_DAC_ADDRESS;
			// }
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
		uint8_t v_calib = this->state == STATE_CALIBRATE_VOLTAGE ? 1 : 0;		//Are we calibrating voltage?
		int8_t negate = channel->get_voltage_limit() < 0 ? -1 : 1;		//We assume that a channel will be negative for both voltage and current.  This should always hold true.

		//If we are calibrating right now, HOLD LEFT cancels calibration and loads defaults; PRESS RIGHT goes to next calibration value; Encoder changes calibration.
		if (this->calibrate_index < CALIBRATION_COUNT){
			if (button1.releaseEvent()){	//Push Button 1 to go back one calibration index
				display.force_reset();
				this->calibrate_index--;
				if (this->calibrate_index > CALIBRATION_COUNT){	//unsigned int wraps to MAX, leave it as zero
					this->calibrate_index = 0;
				}
				// if (v_calib){
				// 	channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index] * negate);
				// }
				// else {
				// 	channel->set_current_setpoint(current_calibration_values[this->calibrate_index] * negate);
				// }
			}
			else if (button2.releaseEvent()){	//Push Button 2 to go forward one calibration index
				display.force_reset();
				this->calibrate_index++;
				// if (this->calibrate_index < CALIBRATION_COUNT){		//If we go past the CALIBRATION_COUNT samples, we show the confirmation message in the display
				// 	if (v_calib){
				// 		channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index] * negate);
				// 	}
				// 	else {
				// 		channel->set_current_setpoint(current_calibration_values[this->calibrate_index] * negate);
				// 	}
				// }
			}
			else {
				//Update the calibration value
				calibration_t calibration = v_calib ? channel->get_calibration_voltage(this->calibrate_index) : channel->get_calibration_current(this->calibrate_index);
				if (encoder1_movement){
					calibration.dac += encoder1_movement / 2;
					//
					// char buffer[128];
					// serial.write((uint8_t*) buffer, (uint16_t) snprintf(buffer, sizeof(buffer), "calibration: adc=%d, dac=%d, adjusted=%d\n\r", calibration.adc, calibration.dac, calibration.adjusted));
				}
				//Cancel calibration, and load last saved calibration values, and reset set points to startup values
				else if (button1.longPressEvent()){
					display.force_reset();
					channel->load_calibration();
					channel->set_voltage_setpoint(channel->get_voltage_startup());
					channel->set_current_setpoint(channel->get_current_startup());
					this->state = STATE_MENU;
				}

				//Update the calibration object with current ADC values and target calibration
				calibration.adc = v_calib ? channel->get_voltage_actual_raw() : channel->get_current_actual_raw();
				calibration.adjusted = negate * (v_calib ? voltage_calibration_values[this->calibrate_index] : current_calibration_values[this->calibrate_index]);

				if (v_calib){
					if (channel->get_voltage_setpoint_raw() != calibration.dac){
						channel->set_voltage_setpoint_raw(calibration.dac);
					}
					channel->set_calibration_voltage(this->calibrate_index, calibration);
				}
				else {
					if (channel->get_current_setpoint_raw() != calibration.dac){
						channel->set_current_setpoint_raw(calibration.dac);
					}
					channel->set_calibration_current(this->calibrate_index, calibration);
				}
			}
		}
		//If we have finished calibration, HOLD LEFT goes back to beginning of calibration to allow more editing; HOLD RIGHT commits calibration and returns to menu.
		else {
			if (button1.longPressEvent()){
				display.force_reset();
				this->calibrate_index = 0;
				if (v_calib){
					channel->set_voltage_setpoint(voltage_calibration_values[this->calibrate_index]);
				}
				else {
					channel->set_current_setpoint(current_calibration_values[this->calibrate_index]);
				}
			}
			else if (button2.longPressEvent()){
				display.force_reset();
				channel->save_calibration();
				channel->set_voltage_setpoint(channel->get_voltage_startup());
				channel->set_current_setpoint(channel->get_current_startup());
				this->state = STATE_MENU;
			}
		}
	}
	else if (this->state == STATE_STARTUP_SETPOINT){
		Channel* channel = &channels[this->scroll_channel];
		if (encoder1_movement || encoder2_movement){

			//Modify the value
			if (encoder1_movement){
				if (channel->get_voltage_limit() > 0){
					this->voltage_setpoint += this->calculate_delta_v(encoder1_movement);
					if (this->voltage_setpoint <= 0) {
						this->voltage_setpoint = 0;
					}
					else if (this->voltage_setpoint >= channel->get_voltage_limit()) {
						this->voltage_setpoint = channel->get_voltage_limit();
					}
				}
				else {
					this->voltage_setpoint += this->calculate_delta_v(encoder1_movement);
					if (this->voltage_setpoint >= 0) {
						this->voltage_setpoint = 0;
					}
					else if (this->voltage_setpoint <= channel->get_voltage_limit()) {
						this->voltage_setpoint = channel->get_voltage_limit();
					}
				}
			}
			else if (encoder2_movement){
				if (channel->get_current_limit() > 0){
					this->current_setpoint += this->calculate_delta_a(encoder2_movement);
					if (this->current_setpoint <= 0) {
						this->current_setpoint = 0;
					}
					else if (this->current_setpoint >= channel->get_current_limit()) {
						this->current_setpoint = channel->get_current_limit();
					}
				}
				else {
					this->current_setpoint -= this->calculate_delta_a(encoder2_movement);
					if (this->current_setpoint >= 0) {
						this->current_setpoint = 0;
					}
					else if (this->current_setpoint <= channel->get_current_limit()) {
						this->current_setpoint = channel->get_current_limit();
					}
				}
			}
		}
		else if (button1.longPressEvent() || button2.longPressEvent()){
			if (button2.longPressEvent()){	//If we are saving the state, we commit it (which automatically persists)
				channel->set_voltage_startup(this->voltage_setpoint);
				channel->set_current_startup(this->current_setpoint);
			}
			display.force_reset();
			this->state = STATE_EDIT_ITEM;
			channel->set_voltage_setpoint(channel->get_voltage_startup());
			channel->set_current_setpoint(channel->get_current_startup());
		}
	}
	else if (this->state == STATE_SET_AREF_SOURCE){
		//TODO If we long press button2 in this state, we will set DAC2 output 3 to always send a high value.  This should be more stable than
		// the +5 VCC voltage, for more accurate measurements.  You only need to do this once, on initial setup of the power supply.
		// For now, this menu is a no-op.
/*
		//If jumper R59 is populated on the board, AREF is connected directly to ISET5.  This is connected to DAC 2, output 3.
		uint8_t dac_channel = 2;
		uint8_t dac_number = 3;

		uint8_t message[3];
		message[0] = DAC_COMMAND_REGISTER_EEPROM | (dac_channel << 1);		//Single write with EEPROM persist
		message[1] = 0x9F;	//First nibble is [VREF,PD1,PD0,Gx].  Set VREF and Gx high.  Second nibble is all high (max value)
		message[2] = 0xFF;	//this byte is all high (max value)
		twi_write_to(DAC_ADDRESS_0 + dac_number, message, 3, TWI_BLOCK, TWI_STOP);
*/
		this->state = STATE_MENU;
	}
	else if (this->state == STATE_SET_DAC_ADDRESS){
		//TODO You need this to set up a new power supply, so that you can set the DAC I2C addresses.  This used to happen from the USB
		// calibration script, but RawHID seems to have broken at some point.  If you are making a new power supply you will need to enable this
		// and finish the implementation.  For now, this menu is a no-op.
		//To enable the address, you need to connect a jumper from pin B0 to the test point on the controller labeled 'I2C_PROG' for the DAC
		// in question.
/*
		uint8_t old_dac_number = 0;		//I think that the factory DAC address is 0, although you can order different chips with different addresses set.
		uint8_t new_dac_number = 1;		//The new DAC address to set.
		if (old_dac_number > 2 || new_dac_number > 2){
			return;			//Don't be stupid, yo!
		}

		DDRB |= _BV(PORTB0);
		PORTB |= _BV(PORTB0);


		//This is a hack.
		// The MCP4728 uses a weird, non-standard I2C message to set the address bits.  It uses LDAC as
		// a selector, and this must be toggled from high to low during the low pulse of the 8th bit
		// in the clock of byte 2.  Obviously our I2C library does not support this, so we need to fake
		// it.  Start the I2C command with the option 'NO BLOCK', and delay for a specific amount of
		// time.  After the delay, bring the LDAC line low.  Timing is critical here; from experiments
		// I found that the right time is just under 40us, but this may differ a bit.  It is recommended
		// that you use a logic probe on SDA, SCL, and LDAC to verify the timing, and adjust this delay
		// as needed.
		// The good news is that this only needs to happen once (per DAC).
		uint8_t message[3];
		message[0] = DAC_COMMAND_ADDRESS | old_dac_number << 2 | 1;		//Command 0x60 + current address (left shifted 2) + 1
		message[1] = DAC_COMMAND_ADDRESS | new_dac_number << 2 | 2;		//Command 0x60 + current address (left shifted 2) + 2
		message[2] = DAC_COMMAND_ADDRESS | new_dac_number << 2 | 3;		//Command 0x60 + new address (left shifted 2) + 3
		twi_write_to(DAC_ADDRESS_0 + old_dac_number, message, 3, TWI_NO_BLOCK, TWI_STOP);

		_delay_us(39.6);
		PORTB &= ~_BV(PORTB0);

		_delay_ms(100);

		usb_send_dac_confirmation(new_dac_number);

		DDRB &= ~_BV(PORTB0);
*/
		this->state = STATE_MENU;
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

int16_t State::get_voltage_setpoint(){
	return this->voltage_setpoint;
}

int16_t State::get_current_setpoint(){
	return this->current_setpoint;
}
