#include "Direct.h"

#include "../Chiindii.h"

using namespace digitalcave;

#ifdef DEBUG
#include <SerialUSB.h>
extern SerialUSB usb;
#endif

Direct::Direct(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void Direct::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_ARMED) {
// #ifdef DEBUG
// 		char temp[32];
// 		uint8_t size = snprintf(temp, sizeof(temp), "Arming: %d\n", request->getData()[0]);
// 		usb.write((uint8_t*) temp, size);
// #endif
		chiindii->setMode(request->getData()[0]);
	}
	else if (cmd == MESSAGE_THROTTLE){
		double* data = (double*) request->getData();
		chiindii->setThrottle(data[0]);
// #ifdef DEBUG
// 		char temp[32];
// 		uint8_t size = snprintf(temp, sizeof(temp), "Throttle: %X%X%X%X\n", request->getData()[0], request->getData()[1], request->getData()[2], request->getData()[3]);
// 		usb.write((uint8_t*) temp, size);
// #endif
	}
	else if (cmd == MESSAGE_ANGLE){
		vector_t* sp = chiindii->getAngleSp();
		double* data = (double*) request->getData();
		sp->x = data[0];
		sp->y = data[1];
		sp->z = data[2];
		
		chiindii->getRateSp()->z = 0;
	}
	else if (cmd == MESSAGE_RATE){
		vector_t* sp = chiindii->getRateSp();
		double* data = (double*) request->getData();
		sp->x = data[0];
		sp->y = data[1];
		sp->z = data[2];
// #ifdef DEBUG
// 		char temp[32];
// 		uint8_t size = snprintf(temp, sizeof(temp), "Rate: %3.2f, %3.2f, %3.2f\n", sp->x, sp->y, sp->z);
// 		usb.write((uint8_t*) temp, size);
// #endif

	}

}