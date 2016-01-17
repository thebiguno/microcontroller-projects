#include "General.h"

#include "../Chiindii.h"

using namespace digitalcave;

General::General(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void General::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_REQUEST_BATTERY){
		uint8_t data[] = { chiindii->getBatteryLevel() };
		FramedSerialMessage response(MESSAGE_SEND_BATTERY, data, 1);
		chiindii->sendMessage(&response);
	}

}
