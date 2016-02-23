#include "General.h"

#include "../Chiindii.h"

using namespace digitalcave;

General::General(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void General::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_REQUEST_DEBUG) {
		chiindii->setDebug(request->getData()[0]);
		if (request->getData()[0]){
			chiindii->sendDebug("Debug Enabled");
		}
		else {
			chiindii->sendDebug("Debug Disabled");
		}
	}
	else if (cmd == MESSAGE_REQUEST_BATTERY){
		uint8_t data[] = { chiindii->getBatteryPercent() };
		FramedSerialMessage response(MESSAGE_SEND_BATTERY, data, 1);
		chiindii->sendMessage(&response);
	}

}
