#include "calibration.h"

extern Leg legs[LEG_COUNT];

extern volatile uint8_t controller;

static volatile uint8_t mode = 0x00;

static volatile uint8_t saveCalibration = 0x00;
static volatile uint8_t loadCalibration = 0x00;
static volatile uint8_t resetCalibration = 0x00;
static volatile uint8_t requestJointCalibration = 0x00;
static volatile uint8_t requestFootCalibration = 0x00;
static volatile uint8_t requestMagnetometerCalibration = 0x00;

void doSaveCalibration(){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
			eeprom_update_byte((uint8_t*) (l * CALIBRATION_COUNT) + j, legs[l].getCalibration(j));
		}
	}

	int16_t x;
	int16_t y;
	magnetometer_get_offsets(&x, &y);
	eeprom_update_word((uint16_t*) MAGNETOMETER_EEPROM_BASE, (uint16_t) x);
	eeprom_update_word((uint16_t*) (MAGNETOMETER_EEPROM_BASE + MAGNETOMETER_EEPROM_OFFSET), (uint16_t) y);	
}

void doLoadCalibration(){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
		}
	}
	
	int16_t x = eeprom_read_word((uint16_t*) MAGNETOMETER_EEPROM_BASE);
	int16_t y = eeprom_read_word((uint16_t*) (MAGNETOMETER_EEPROM_BASE + MAGNETOMETER_EEPROM_OFFSET));
	magnetometer_set_offsets(x, y);
}

void doResetCalibration(){
	if (resetCalibration == MODE_CALIBRATION_JOINTS){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				legs[l].setCalibration(j, 0);
			}
		}
	}
	else if (resetCalibration == MODE_CALIBRATION_FEET){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				legs[l].setCalibration(j + JOINT_COUNT, 0);
			}
		}
	}
	else if (resetCalibration == MODE_CALIBRATION_MAGNETOMETER){
		magnetometer_set_offsets(0, 0);
	}	
}

void calibration_command_executor(){
	battery_enable_status();
	pwm_start();

	while (controller == CONTROLLER_CALIBRATION){
		wdt_reset();
		
		//Different modes use different foot positions
		if (mode == MODE_CALIBRATION_JOINTS){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[l].getCalibration(j) * 10));
				}
			}
			pwm_apply_batch();
			mode = 0x00;
		}
		else if (mode) {
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				legs[l].setOffset(Point(0,0,0));
			}
			pwm_apply_batch();
			mode = 0x00;
		}
		
		//Handle persistence requests
		if (saveCalibration){
			doSaveCalibration();
			saveCalibration = 0x00;
		}
		else if (loadCalibration){
			doLoadCalibration();
			loadCalibration = 0x00;
		}
		else if (resetCalibration){
			doResetCalibration();
			resetCalibration = 0x00;
		}
		
		//Send current values if requested
		if (requestJointCalibration){
			int8_t message[18];
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					message[(l * JOINT_COUNT) + j] = legs[l].getCalibration(j);
				}
			}

			protocol_send_message(MESSAGE_SEND_JOINT_CALIBRATION, (uint8_t*) message, 18);
			requestJointCalibration = 0x00;
		}
		else if (requestFootCalibration){
			int8_t message[18];
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					message[(l * JOINT_COUNT) + j] = legs[l].getCalibration(j + JOINT_COUNT);
				}
			}

			protocol_send_message(MESSAGE_SEND_FOOT_CALIBRATION, (uint8_t*) message, 18);
			requestFootCalibration = 0x00;
		}
		else if (requestMagnetometerCalibration){
			int16_t x;
			int16_t y;
			magnetometer_get_offsets(&x, &y);
			uint8_t message[4];
			message[0] = (x >> 8) & 0xFF;
			message[1] = x & 0xFF;
			message[2] = (y >> 8) & 0xFF;
			message[3] = y & 0xFF;
			protocol_send_message(MESSAGE_SEND_MAGNETOMETER_CALIBRATION, message, 4);
			requestMagnetometerCalibration = 0x00;
		}
		
		delay_ms(20);
	}

	//Reload calibration from EEPROM; if it was committed, this will do nothing,
	// otherwise we revert any changes done in this session.
	doLoadCalibration();
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	status_set_color(0x00, 0x00, 0x00);
	delay_ms(200);
	
	pwm_stop();
}

void calibration_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_SAVE_CALIBRATION){
		saveCalibration = 0x01;
	}
	else if (cmd == MESSAGE_LOAD_CALIBRATION){
		loadCalibration = 0x01;
	}
	else if (cmd == MESSAGE_RESET_CALIBRATION){
		resetCalibration = message[0];
	}
	else if (cmd == MESSAGE_REQUEST_JOINT_CALIBRATION){
		requestJointCalibration = 0x01;
		mode = MODE_CALIBRATION_JOINTS;
	}
	else if (cmd == MESSAGE_SEND_JOINT_CALIBRATION){
		if (length == 18){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					legs[l].setCalibration(j, message[(l * JOINT_COUNT) + j]);
				}
			}
			mode = MODE_CALIBRATION_JOINTS;
		}
	}
	else if (cmd == MESSAGE_REQUEST_FOOT_CALIBRATION){
		requestFootCalibration = 0x01;
		mode = MODE_CALIBRATION_FEET;
	}
	else if (cmd == MESSAGE_SEND_FOOT_CALIBRATION){
		if (length == 18){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					legs[l].setCalibration(j + CALIBRATION_COUNT, message[(l * JOINT_COUNT) + j]);
				}
			}
			
			mode = MODE_CALIBRATION_FEET;
		}
	}
	else if (cmd == MESSAGE_REQUEST_MAGNETOMETER_CALIBRATION){
		requestMagnetometerCalibration = 0x01;
		mode = MODE_CALIBRATION_MAGNETOMETER;
	}
	else if (cmd == MESSAGE_SEND_MAGNETOMETER_CALIBRATION){
		if (length == 4){
			int16_t x = (int16_t) ((message[0] << 8) | message[1]);
			int16_t y = (int16_t) ((message[2] << 8) | message[3]);
			magnetometer_set_offsets(x, y);
			
			mode = MODE_CALIBRATION_MAGNETOMETER;
		}
	}
}