#include "Calibration.h"

#include "../Stubby.h"

using namespace digitalcave;

Calibration::Calibration(Stubby* stubby) :
	stubby(stubby)
{
	;
}

void Calibration::dispatch(FramedSerialMessage* message){
	if (message->getCommand() == MESSAGE_SAVE_CALIBRATION){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
				eeprom_update_byte((uint8_t*) (l * CALIBRATION_COUNT) + j, stubby->getLegs()[l]->getCalibration(j));
			}
		}

		//int16_t x;
		//int16_t y;
		//magnetometer_get_offsets(&x, &y);
		//eeprom_update_word((uint16_t*) MAGNETOMETER_EEPROM_BASE, (uint16_t) x);
		//eeprom_update_word((uint16_t*) (MAGNETOMETER_EEPROM_BASE + MAGNETOMETER_EEPROM_OFFSET), (uint16_t) y);
	}
	else if (message->getCommand() == MESSAGE_LOAD_CALIBRATION){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
				stubby->getLegs()[l]->setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
			}
		}

		//int16_t x = eeprom_read_word((uint16_t*) MAGNETOMETER_EEPROM_BASE);
		//int16_t y = eeprom_read_word((uint16_t*) (MAGNETOMETER_EEPROM_BASE + MAGNETOMETER_EEPROM_OFFSET));
		//magnetometer_set_offsets(x, y);
	}
	else if (message->getCommand() == MESSAGE_RESET_CALIBRATION){
		uint8_t resetCalibration = message->getData()[0];

		if (resetCalibration == MODE_CALIBRATION_JOINTS){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					stubby->getLegs()[l]->setCalibration(j, 0);
					pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (stubby->getLegs()[l]->getCalibration(j) * 10));
				}
			}
			pwm_apply_batch();
		}
		else if (resetCalibration == MODE_CALIBRATION_FEET){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					stubby->getLegs()[l]->setCalibration(j + JOINT_COUNT, 0);
				}
			}
		}
		else if (resetCalibration == MODE_CALIBRATION_MAGNETOMETER){
			//magnetometer_set_offsets(0, 0);
		}

	}
	else if (message->getCommand() == MESSAGE_REQUEST_JOINT_CALIBRATION){
		int8_t message[18];
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				message[(l * JOINT_COUNT) + j] = stubby->getLegs()[l]->getCalibration(j);
			}
		}

		FramedSerialMessage response(MESSAGE_SEND_JOINT_CALIBRATION, (uint8_t*) message, 18);
		stubby->sendMessage(&response);
	}
	else if (message->getCommand() == MESSAGE_SEND_JOINT_CALIBRATION){
		if (message->getLength() == 18){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					stubby->getLegs()[l]->setCalibration(j, message->getData()[(l * JOINT_COUNT) + j]);
				}
			}
		}
	}
	else if (message->getCommand() == MESSAGE_REQUEST_FOOT_CALIBRATION){
		int8_t message[18];
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				message[(l * JOINT_COUNT) + j] = stubby->getLegs()[l]->getCalibration(j + JOINT_COUNT);
			}
		}

		FramedSerialMessage response(MESSAGE_SEND_FOOT_CALIBRATION, (uint8_t*) message, 18);
		stubby->sendMessage(&response);
	}
	else if (message->getCommand() == MESSAGE_SEND_FOOT_CALIBRATION){
		if (message->getLength() == 18){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				for (uint8_t j = 0; j < JOINT_COUNT; j++){
					stubby->getLegs()[l]->setCalibration(j + JOINT_COUNT, message->getData()[(l * JOINT_COUNT) + j]);
				}
			}
		}
	}
	// else if (message->getCommand() == MESSAGE_REQUEST_MAGNETOMETER_CALIBRATION){
	// 	int16_t x;
	// 	int16_t y;
	// 	magnetometer_get_offsets(&x, &y);
	// 	uint8_t message[4];
	// 	message[0] = (x >> 8) & 0xFF;
	// 	message[1] = x & 0xFF;
	// 	message[2] = (y >> 8) & 0xFF;
	// 	message[3] = y & 0xFF;
	// 	FramedSerialMessage response(MESSAGE_SEND_MAGNETOMETER_CALIBRATION, (uint8_t*) message, 4);
	// 	stubby->sendMessage(&response);
	// }
	// else if (message->getCommand() == MESSAGE_SEND_MAGNETOMETER_CALIBRATION){
	// 	if (message->getLength() == 4){
	// 		int16_t x = (int16_t) ((message->getData()[0] << 8) | message->getData()[1]);
	// 		int16_t y = (int16_t) ((message->getData()[2] << 8) | message->getData()[3]);
	// 		magnetometer_set_offsets(x, y);
	// 	}
	// }
	// else if (message->getCommand() == MESSAGE_START_MAGNETOMETER_CALIBRATION){
	// 	uint8_t step_index = 0;
	//
	// 	//delay_ms(200);
	// 	//Rotate slowly in place, constantly sending magnetometer readings to the computer.  This
	// 	// many iterations should be about 3 - 5 full turns.
	// 	for (uint16_t i = 0; i < 2000; i++){
	// 		wdt_reset();
	// 		for (uint8_t l = 0; l < LEG_COUNT; l++){
	// 			Point step = gait_step(stubby->getLegs()[l], step_index, 0, 0, 0.5);
	// 			stubby->getLegs()[l]->setOffset(step);
	// 		}
	// 		pwm_apply_batch();
	//
	// 		step_index++;
	// 		if (step_index > gait_step_count()){
	// 			step_index = 0;
	// 		}
	//
	// 		//delay_ms(20);
	//
	// 		//Send magnetometer reading
	// 		int16_t x;
	// 		int16_t y;
	// 		magnetometer_get_raw(&x, &y);
	// 		uint8_t message[4];
	// 		message[0] = (x >> 8) & 0xFF;
	// 		message[1] = x & 0xFF;
	// 		message[2] = (y >> 8) & 0xFF;
	// 		message[3] = y & 0xFF;
	// 		FramedSerialMessage response(MESSAGE_SEND_MAGNETOMETER_CALIBRATION, (uint8_t*) message, 4);
	// 		stubby->getProtocol()->write(serial, &response);
	// 	}
	// }
}
