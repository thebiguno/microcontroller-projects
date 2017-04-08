#include "Calibration.h"

#include <avr/wdt.h>

#include "../Chiindii.h"
#include "../timer/timer.h"

using namespace digitalcave;

#ifdef DEBUG
char temp[128];
#endif

Calibration::Calibration(Chiindii* chiindii) {
	this->chiindii = chiindii;
}

void Calibration::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	
	if (cmd == MESSAGE_SAVE_CALIBRATION){
		chiindii->saveConfig();
	}
	else if (cmd == MESSAGE_LOAD_CALIBRATION){
		chiindii->loadConfig();
	}
	else if (cmd == MESSAGE_CALIBRATE_IMU){
		wdt_enable(WDTO_8S);	//This takes a bit of time... we need to make sure the WDT doesn't reset.
		chiindii->getMpu6050()->calibrate();
		wdt_enable(WDTO_120MS);
	}
#if defined MAHONY
	//TODO Add support for Mahoney tuning
#elif defined MADGWICK
	else if (cmd == MESSAGE_MADGWICK_TUNING){
		Madgwick* m = chiindii->getImu();
		if (request->getLength() == 0){
			double data[] = { m->getBeta() };
			FramedSerialMessage response(MESSAGE_MADGWICK_TUNING, (uint8_t*) data, 4);
			chiindii->sendMessage(&response);
		}
		else {
			double* data = (double*) request->getData();
			m->setBeta(data[0]);
		}
	}
#endif
	else if (cmd == MESSAGE_RATE_PID_TUNING){
		PID* x = chiindii->getRateX();
		PID* y = chiindii->getRateY();
		PID* z = chiindii->getRateZ();
		if (request->getLength() == 0){
			float data[9];
			x->getTunings(&data[0], &data[1], &data[2]);
			y->getTunings(&data[3], &data[4], &data[5]);
			z->getTunings(&data[6], &data[7], &data[8]);
			FramedSerialMessage response(MESSAGE_RATE_PID_TUNING, (uint8_t*) data, 36);
			chiindii->sendMessage(&response);
		}
		else {
			double* data = (double*) request->getData();
			x->setTunings(data[0], data[1], data[2]);
			y->setTunings(data[3], data[4], data[5]);
			z->setTunings(data[6], data[7], data[8]);
		}
	}
	else if (cmd == MESSAGE_ANGLE_PID_TUNING){
		PID* x = chiindii->getAngleX();
		PID* y = chiindii->getAngleY();
		PID* z = chiindii->getAngleZ();
		if (request->getLength() == 0){
			float data[9];
			x->getTunings(&data[0], &data[1], &data[2]);
			y->getTunings(&data[3], &data[4], &data[5]);
			z->getTunings(&data[6], &data[7], &data[8]);
			FramedSerialMessage response(MESSAGE_ANGLE_PID_TUNING, (uint8_t*) data, 36);
			chiindii->sendMessage(&response);
		}
		else {
			double* data = (double*) request->getData();
			x->setTunings(data[0], data[1], data[2]);
			y->setTunings(data[3], data[4], data[5]);
			z->setTunings(data[6], data[7], data[8]);
		}
	}
	else if (cmd == MESSAGE_THROTTLE_PID_TUNING){
		PID* t = chiindii->getGforce();
		if (request->getLength() == 0){
			float data[3];
			t->getTunings(&data[0], &data[1], &data[2]);
			FramedSerialMessage response(MESSAGE_THROTTLE_PID_TUNING, (uint8_t*) data, 12);
			chiindii->sendMessage(&response);
		}
		else {
			double* data = (double*) request->getData();
			t->setTunings(data[0], data[1], data[2]);
		}
	}
}
