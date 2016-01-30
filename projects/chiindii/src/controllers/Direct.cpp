#include "Direct.h"

#include "../Chiindii.h"

using namespace digitalcave;

Direct::Direct(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void Direct::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_ARMED) {
#ifdef DEBUG
		char temp[32];
		uint8_t size = snprintf(temp, sizeof(temp), "Arming: %d\n", request->getData()[0]);
		usb_serial_write((const uint8_t*) temp, size);
#endif
		chiindii->setMode(request->getData()[0]);
	}
	else if (cmd == MESSAGE_THROTTLE){
		double* data = (double*) request->getData();
		chiindii->setThrottle(data[0]);
	}
	else if (cmd == MESSAGE_ANGLE){
		vector_t* sp = chiindii->getAngleSp();
		double* data = (double*) request->getData();
		sp->x = data[0];
		sp->y = data[1];
		chiindii->getRateSp()->z = data[2];
	}
	else if (cmd == MESSAGE_RATE){
		vector_t* sp = chiindii->getRateSp();
		double* data = (double*) request->getData();
		sp->x = data[0];
		sp->y = data[1];
		sp->z = data[2];
	}
	else if (cmd == MESSAGE_LEVEL){
		chiindii->getMpu6050()->calibrate();
	}

}
