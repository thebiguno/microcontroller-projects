#include "Direct.h"

#include "../Chiindii.h"

using namespace digitalcave;

Direct::Direct(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void Direct::dispatch(FramedSerialMessage* message) {
	uint8_t cmd = message->getCommand();
	if (cmd == MESSAGE_THROTTLE){
		double* data = (double*) message->getData();
		chiindii->setThrottle(data[0]);
	}
	else if (cmd == MESSAGE_ANGLE){
		vector_t* sp = chiindii->getAngleSp();
		double* data = (double*) message->getData();
		sp->x = data[0];
		sp->y = data[1];
		sp->z = data[2];
	}
	else if (cmd == MESSAGE_RATE){
		vector_t* sp = chiindii->getRateSp();
		double* data = (double*) message->getData();
		sp->x = data[0];
		sp->y = data[1];
		sp->z = data[2];
	}
}
