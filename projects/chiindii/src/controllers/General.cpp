#include "General.h"

#include "../Chiindii.h"

using namespace digitalcave;

General::General(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void General::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_REQUEST_ENABLE_DEBUG) {
		chiindii->setDebug(0x01);
		FramedSerialMessage response(MESSAGE_REQUEST_ENABLE_DEBUG, 0);
		chiindii->sendMessage(&response);
	}
	else if (cmd == MESSAGE_REQUEST_DISABLE_DEBUG) {
		chiindii->setDebug(0x00);
		FramedSerialMessage response(MESSAGE_REQUEST_DISABLE_DEBUG, 0);
		chiindii->sendMessage(&response);
	}
	else if (cmd == MESSAGE_REQUEST_BATTERY){
		uint8_t data[] = { chiindii->getBatteryPercent() };
		FramedSerialMessage response(MESSAGE_SEND_BATTERY, data, 1);
		chiindii->sendMessage(&response);
	}

}
