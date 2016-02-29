#include "General.h"

#include "../Chiindii.h"

using namespace digitalcave;

General::General(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void General::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_DEBUG) {
		chiindii->setDebug(request->getData()[0]);
		if (request->getData()[0]){
			chiindii->sendDebug("Debug Enabled ", 14);
		}
		else {
			chiindii->sendDebug("Debug Disabled", 14);
		}
	}
	else if (cmd == MESSAGE_BATTERY){
		uint8_t data[] = { chiindii->getBatteryPercent() };
		FramedSerialMessage response(MESSAGE_BATTERY, data, 1);
		chiindii->sendMessage(&response);
	}

}
