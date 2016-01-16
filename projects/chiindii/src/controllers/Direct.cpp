#include "Direct.h"

using namespace digitalcave;

Direct::Direct(Chiindii *chiindii) {
	this->chiindii = chiindii;
}

void Direct::dispatch(FramedSerialMessage* message) {
	uint8_t cmd = message.getCommand();
	if (cmd == MESSAGE_THROTTLE){
		// TODO
	}
	else if (cmd == MESSAGE_ANGLE){
		double[] data = (double[]) message.getData();
		angle_sp.setX(data[0]);
		angle_sp.setY(data[1]);
		angle_sp.setZ(data[2]);
	}
	else if (cmd == MESSAGE_RATE){
		double[] data = (double[]) message.getData();
		rate_sp.setX(data[0]);
		rate_sp.setY(data[1]);
		rate_sp.setZ(data[2]);
	}
}
