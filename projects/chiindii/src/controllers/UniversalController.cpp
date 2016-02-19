#include "UniversalController.h"

#include "../Chiindii.h"

using namespace digitalcave;

UniversalController::UniversalController(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void UniversalController::dispatch(FramedSerialMessage* message) {
	uint8_t cmd = message->getCommand();
	if (cmd == MESSAGE_UC_JOYSTICK_MOVE) {
		if (chiindii->getMode() == MODE_ARMED_THROTTLE){
			uint8_t* sticks = message->getData();
			//uint8_t lx = sticks[0];
			uint8_t rx = sticks[2];
			uint8_t ry = sticks[3];
		
			vector_t* angle_sp = chiindii->getAngleSp();
			vector_t* rate_sp = chiindii->getRateSp();
			//For the joysticks, we read 10 degrees from each side (with 0.1 degree resolution),
			// and leave the remainder in the center unused to allow for slop in the center readings
			if (rx < 100){
				angle_sp->x = -1 * ((100.0 - rx) / 10) * M_PI / 180.0;
			}
			else if (rx > 155){
				angle_sp->x = ((rx - 155.0) / 10) * M_PI / 180.0;
			}
			else {
				angle_sp->x = 0;
			}
		
			if (ry < 100){
				angle_sp->y = ((100.0 - ry) / 10) * M_PI / 180.0;
			}
			else if (ry > 155){
				angle_sp->y = -1 * ((ry - 155.0) / 10) * M_PI / 180.0;
			}
			else {
				angle_sp->y = 0;
			}
			
// 			if (lx < 100){
// 				rate_sp->z = ((100 - lx) / 10) * M_PI / 180.0;
// 			}
// 			else if (ry > 155){
// 				angle_sp->z = ((lx - 155) / 10) * M_PI / 180.0;
// 			}
// 			else {
				rate_sp->z = 0;
// 			}
		}
	}
	else if (cmd == MESSAGE_UC_THROTTLE_MOVE){
		chiindii->setThrottle(message->getData()[0] / 255.0);
// 		char temp[14];
// 		snprintf(temp, sizeof(temp), "Throttle: %d", (uint8_t) (chiindii->getThrottle() * 100));
// 		chiindii->sendDebug(temp);
	}
	else if (cmd == MESSAGE_UC_BUTTON_PUSH){
		//To arm, press the triangle (top discrete) button while the throttle is all the way down.
		if (message->getData()[0] == CONTROLLER_BUTTON_VALUE_TRIANGLE && chiindii->getThrottle() < 0.01){
			chiindii->sendDebug("Armed         ");
			chiindii->setMode(MODE_ARMED_THROTTLE);
		}
		//To disarm, press the cross (bottom discrete) button at any time
		else if (message->getData()[0] == CONTROLLER_BUTTON_VALUE_CROSS){
			chiindii->sendDebug("Disarmed      ");
			chiindii->setMode(MODE_UNARMED);
			chiindii->setThrottle(0);
		}
	}
}
