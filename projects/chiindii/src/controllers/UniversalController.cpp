#include "UniversalController.h"

#include "../Chiindii.h"

using namespace digitalcave;

UniversalController::UniversalController(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void UniversalController::dispatch(FramedSerialMessage* message) {
	uint8_t cmd = message->getCommand();
	if (cmd == MESSAGE_UC_JOYSTICK_MOVE) {
		if (chiindii->getMode() == MODE_ARMED_ANGLE){
			uint8_t* sticks = message->getData();
			uint8_t rx = sticks[2];
			uint8_t ry = sticks[3];
		
			vector_t* sp = chiindii->getAngleSp();
			//For the joysticks, we read 10 degrees from each side (with 0.1 degree resolution),
			// and leave the remainder in the center unused to allow for slop in the center readings
			if (rx < 100){
				sp->x = ((100 - rx) / 10) * M_PI / 180.0;
			}
			else if (rx > 155){
				sp->x = ((rx - 155) / 10) * M_PI / 180.0;
			}
			else {
				sp->x = 0;
			}
		
			if (ry < 100){
				sp->y = ((100 - ry) / 10) * M_PI / 180.0;
			}
			else if (ry > 155){
				sp->y = ((ry - 155) / 10) * M_PI / 180.0;
			}
			else {
				sp->y = 0;
			}
		}
	}
	else if (cmd == MESSAGE_UC_THROTTLE_MOVE){
		if (chiindii->getMode() != MODE_UNARMED){
			chiindii->setThrottle(message->getData()[0] / 255.0);
		}
	}
	else if (cmd == MESSAGE_UC_BUTTON_PUSH){
		//To arm, press the triangle (top discrete) button while the throttle is all the way down.
		if (message->getData()[0] == CONTROLLER_BUTTON_VALUE_TRIANGLE && chiindii->getMode() != MODE_UNARMED && chiindii->getThrottle() < 0.01){
			chiindii->setMode(MODE_ARMED_ANGLE);
		}
		//To disarm, press the cross (bottom discrete) button at any time
		else if (message->getData()[0] == CONTROLLER_BUTTON_VALUE_CROSS){
			chiindii->setMode(MODE_UNARMED);
			chiindii->setThrottle(0);
		}
	}
}
