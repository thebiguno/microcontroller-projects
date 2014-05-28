#include "calibration.h"

extern leg_t legs[LEG_COUNT];

/*
 * Calibration mode.  Use the following buttons to calibrate:
 *  -Circle switches between legs.  As a new leg is selected, flash the leg number on the status LED.
 *  -Left / Right adjusts coxa joint for selected leg
 *  -Up / Down adjusts femur joint for selected leg
 *  -L1 / L2 adjusts tibia joint for selected leg
 *  -X commits changes to EEPROM
 *  -Start exits calibration mode (regardless of whether or not it is committed)
 */
void calibration_main(){
	status_disable_timer();
	status_set_color(0xFF, 0xFF, 0x00);			//Yellow
	
	uint8_t l = 0;
	
	uint16_t buttons;
	//Loop until Start is pressed
	while (((buttons = comm_read_buttons()) & _BV(CONTROLLER_BUTTON_VALUE_START)) == 0){
		//Circle increments legs.
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0) {
			l++;
			if (l >= LEG_COUNT) l = 0;
			
			//Flash red to indicate the leg count.
			for (uint8_t i = 0; i <= l; l++){
				status_set_color(0xFF, 0x00, 0x00);			//Red
				_delay_ms(100);
				status_set_color(0x00, 0x00, 0x00);			//Black
				_delay_ms(200);
			}
			_delay_ms(200);
			status_set_color(0xFF, 0xFF, 0x00);			//Yellow
		}
		
		//Left / Right arrows adjust coxa joint
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
			legs[l].offset[COXA]++;
			servo_set_angle(l, COXA, 0);
		}
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
			legs[l].offset[COXA]--;
			servo_set_angle(l, COXA, 0);
		}
		
		//Up / Down arrows adjust femur joint
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
			legs[l].offset[FEMUR]++;
			servo_set_angle(l, FEMUR, 0);
		}
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
			legs[l].offset[FEMUR]--;
			servo_set_angle(l, FEMUR, 0);
		}
		
		//L1 / L2 buttons adjust the tibia joint
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
			legs[l].offset[TIBIA]++;
			servo_set_angle(l, TIBIA, 0);
		}
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
			legs[l].offset[TIBIA]--;
			servo_set_angle(l, TIBIA, 0);
		}
		
		//X button to commit changes to EEPROM
		if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
			servo_save_calibration();
			//Flash green to indicate save
			status_set_color(0x00, 0xFF, 0x00);			//Green
			_delay_ms(500);
			status_set_color(0xFF, 0xFF, 0x00);			//Yellow
		}
	}
	
	//Reload calibration from EEPROM; if it was committed, this will essentially
	// be a NOP, otherwise we revert any changes done in this session.
	servo_load_calibration();
	
	status_enable_timer();
}