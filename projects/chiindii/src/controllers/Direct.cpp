#include "Direct.h"

#include "../Chiindii.h"

using namespace digitalcave;

Direct::Direct(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void Direct::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	if (cmd == MESSAGE_TOGGLE_ARMED) {
		if (chiindii->getMode() == MODE_UNARMED) {
			chiindii->setMode(MODE_ARMED_ANGLE);
		} else {
			chiindii->setMode(MODE_UNARMED);
		}
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
		
		if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
			chiindii->setMode(MODE_UNARMED);
		} else {
			chiindii->setMode(MODE_ARMED_RATE);
		}
	}
	else if (cmd == MESSAGE_ZERO){
		chiindii->getMpu6050()->calibrate();
	}

}
