#include "General.h"

#include "../Stubby.h"

using namespace digitalcave;

General::General(Stubby* stubby) :
	stubby(stubby)
{
	;
}

void General::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_BATTERY){
		uint8_t data[] = { stubby->getBatteryPercent() };
		FramedSerialMessage response(MESSAGE_BATTERY, data, 1);
		stubby->sendMessage(&response);
	}

}
