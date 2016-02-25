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

void Calibration::read() {
	if (eeprom_read_byte((uint8_t*) EEPROM_MAGIC) == 0x42){
		double kp, ki, kd;
		double t;
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 0));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 4));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 8));
		chiindii->getRateX()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 12));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 16));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 20));
		chiindii->getRateY()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 24));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 28));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 32));
		chiindii->getRateZ()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 36));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 40));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 44));
		chiindii->getAngleX()->setTunings(kp, ki, kd);

		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 48));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 52));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 56));
		chiindii->getAngleY()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 60));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 64));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 68));
		chiindii->getAngleZ()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 72));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 76));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 80));
		chiindii->getGforce()->setTunings(kp, ki, kd);

		t = eeprom_read_float((float*) (EEPROM_OFFSET + 84));
		chiindii->getMadgwick()->setBeta(t);
		
		//6 * 2 bytes = 12 bytes total for accel + gyro calibration
		int16_t calibration[3];
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 88), 6);
		chiindii->getMpu6050()->setAccelCalib(calibration);
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 94), 6);
		chiindii->getMpu6050()->setGyroCalib(calibration);
#ifdef DEBUG
		chiindii->sendStatus("Load EEPROM   ");
#endif
	}
	else {
#ifdef DEBUG
		chiindii->sendStatus("Load Defaults ");
#endif
	}
}

void Calibration::write() {
	cli();
	wdt_disable();
	
	PID* rate_x = chiindii->getRateX();
	eeprom_update_float((float*) (EEPROM_OFFSET + 0), rate_x->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 4), rate_x->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 8), rate_x->getKd());

	PID* rate_y = chiindii->getRateY();
	eeprom_update_float((float*) (EEPROM_OFFSET + 12), rate_y->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 16), rate_y->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 20), rate_y->getKd());
	
	PID* rate_z = chiindii->getRateZ();
	eeprom_update_float((float*) (EEPROM_OFFSET + 24), rate_z->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 28), rate_z->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 32), rate_z->getKd());

	PID* angle_x = chiindii->getAngleX();
	eeprom_update_float((float*) (EEPROM_OFFSET + 36), angle_x->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 40), angle_x->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 44), angle_x->getKd());

	PID* angle_y = chiindii->getAngleY();
	eeprom_update_float((float*) (EEPROM_OFFSET + 48), angle_y->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 52), angle_y->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 56), angle_y->getKd());

	PID* angle_z = chiindii->getAngleZ();
	eeprom_update_float((float*) (EEPROM_OFFSET + 60), angle_z->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 64), angle_z->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 68), angle_z->getKd());

	PID* gforce = chiindii->getGforce();
	eeprom_update_float((float*) (EEPROM_OFFSET + 72), gforce->getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 76), gforce->getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 80), gforce->getKd());

	Madgwick* madgwick = chiindii->getMadgwick();
	eeprom_update_float((float*) (EEPROM_OFFSET + 84), madgwick->getBeta());
	
	eeprom_update_block(chiindii->getMpu6050()->getAccelCalib(), (void*) (EEPROM_OFFSET + 88), 6);
	eeprom_update_block(chiindii->getMpu6050()->getGyroCalib(), (void*) (EEPROM_OFFSET + 94), 6);
	
	//Write the magic value to say that we have written valid bytes
	eeprom_update_byte((uint8_t*) EEPROM_MAGIC, 0x42);
	
	wdt_enable(WDTO_120MS);
	sei();
	
#ifdef DEBUG
	chiindii->sendStatus("Save EEPROM\n");
#endif
}

void Calibration::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	
	if (cmd == MESSAGE_SAVE_CALIBRATION){
		this->write();
	}
	else if (cmd == MESSAGE_LOAD_CALIBRATION){
		this->read();
	}
	else if (cmd == MESSAGE_CALIBRATE_IMU){
		wdt_enable(WDTO_8S);	//This takes a bit of time... we need to make sure the WDT doesn't reset.
		chiindii->getMpu6050()->calibrate();
		wdt_enable(WDTO_120MS);
	}
	else if (cmd == MESSAGE_MADGWICK_TUNING){
		Madgwick* m = chiindii->getMadgwick();
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
	else if (cmd == MESSAGE_RATE_PID_TUNING){
		PID* x = chiindii->getRateX();
		PID* y = chiindii->getRateY();
		PID* z = chiindii->getRateZ();
		if (request->getLength() == 0){
			double data[] = { 
				x->getKp(), x->getKi(), x->getKd(),
				y->getKp(), y->getKi(), y->getKd(),
				z->getKp(), z->getKi(), z->getKd()
			};
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
			double data[] = { 
				x->getKp(), x->getKi(), x->getKd(),
				y->getKp(), y->getKi(), y->getKd(),
				z->getKp(), z->getKi(), z->getKd()
			};
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
			double data[] = { 
				t->getKp(), t->getKi(), t->getKd()
			};
			FramedSerialMessage response(MESSAGE_THROTTLE_PID_TUNING, (uint8_t*) data, 12);
			chiindii->sendMessage(&response);
		}
		else {
			double* data = (double*) request->getData();
			t->setTunings(data[0], data[1], data[2]);
		}
	}
}
