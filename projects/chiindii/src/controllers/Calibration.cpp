#include "Calibration.h"

#include "../Chiindii.h"
#include "../lib/timer/timer.h"


using namespace digitalcave;

/* EEPROM format

 0 rate x p
 4 rate x i
 8 rate x d
12 rate y p
16 rate y i
20 rate y d
24 rate z p
28 rate z i
32 rate z d
36 angle x p
40 angle x i
44 angle x d
48 angle y p
52 angle y i
56 angle y d
60 comp x t
64 comp y t

 */

Calibration::Calibration(Chiindii* chiindii) {
	this->chiindii = chiindii;
}

void Calibration::read() {
	double kp, ki, kd;
	double t;
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 0);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 4);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 8);
	chiindii->getRateX()->setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 12);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 16);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 20);
	chiindii->getRateY()->setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 24);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 28);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 32);
	chiindii->getRateZ()->setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 36);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 40);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 44);
	chiindii->getAngleX()->setTunings(kp, ki, kd);

	kp = eeprom_read_float((float*) EEPROM_OFFSET + 48);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 52);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 56);
	chiindii->getRateY()->setTunings(kp, ki, kd);
	
	t = eeprom_read_float((float*) EEPROM_OFFSET + 60);
	chiindii->getCompX()->setTau(t);
	t = eeprom_read_float((float*) EEPROM_OFFSET + 64);
	chiindii->getCompY()->setTau(t);
}

void Calibration::write() {
	PID* rate_x = chiindii->getRateX();
	eeprom_update_float((float*) EEPROM_OFFSET + 0, rate_x->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 4, rate_x->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 8, rate_x->getKd());

	PID* rate_y = chiindii->getRateY();
	eeprom_update_float((float*) EEPROM_OFFSET + 12, rate_y->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 16, rate_y->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 20, rate_y->getKd());
	
	PID* rate_z = chiindii->getRateZ();
	eeprom_update_float((float*) EEPROM_OFFSET + 24, rate_z->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 28, rate_z->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 32, rate_z->getKd());

	PID* angle_x = chiindii->getAngleX();
	eeprom_update_float((float*) EEPROM_OFFSET + 36, angle_x->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 40, angle_x->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 44, angle_x->getKd());

	PID* angle_y = chiindii->getAngleY();
	eeprom_update_float((float*) EEPROM_OFFSET + 48, angle_y->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 52, angle_y->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 56, angle_y->getKd());

	Complementary* c_x = chiindii->getCompX();
	eeprom_update_float((float*) EEPROM_OFFSET + 60, c_x->getTau());
	
	Complementary* c_y = chiindii->getCompY();
	eeprom_update_float((float*) EEPROM_OFFSET + 64, c_y->getTau());
}

void Calibration::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();
	
	if (cmd == MESSAGE_SAVE_CALIBRATION){
		this->write();
	}
	else if (cmd == MESSAGE_LOAD_CALIBRATION){
		this->read();
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_RATE_PID){
		PID* x = chiindii->getRateX();
		PID* y = chiindii->getRateY();
		PID* z = chiindii->getRateZ();
		double data[] = { 
			x->getKp(), x->getKi(), x->getKd(),
			y->getKp(), y->getKi(), y->getKd(),
			z->getKp(), z->getKi(), z->getKd()
		};
		FramedSerialMessage response(MESSAGE_REQUEST_CALIBRATION_RATE_PID, (uint8_t*) data, 36);
		chiindii->sendMessage(&response);
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_ANGLE_PID){
		PID* x = chiindii->getAngleX();
		PID* y = chiindii->getAngleY();
		double data[] = { 
			x->getKp(), x->getKi(), x->getKd(),
			y->getKp(), y->getKi(), y->getKd()
		};
		FramedSerialMessage response(MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, (uint8_t*) data, 24);
		chiindii->sendMessage(&response);
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY){
		Complementary* x = chiindii->getCompX();
		Complementary* y = chiindii->getCompY();
		double data[] = { 
			x->getTau(), y->getTau()
		};
		FramedSerialMessage response(MESSAGE_REQUEST_CALIBRATION_RATE_PID, (uint8_t*) data, 8);
		chiindii->sendMessage(&response);
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_RATE_PID){
		double* data = (double*) request->getData();
		chiindii->getRateX()->setTunings(data[0], data[1], data[2]);
		chiindii->getRateY()->setTunings(data[3], data[4], data[5]);
		chiindii->getRateZ()->setTunings(data[6], data[7], data[8]);
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_RATE_PID){
		double* data = (double*) request->getData();
		chiindii->getAngleX()->setTunings(data[0], data[1], data[2]);
		chiindii->getAngleY()->setTunings(data[3], data[4], data[5]);
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_COMPLEMENTARY){
		double* data = (double*) request->getData();
		chiindii->getCompX()->setTau(data[0]);
		chiindii->getCompY()->setTau(data[1]);
	}
	else if (cmd == MESSAGE_START_CALIBRATION_COMPLEMENTARY){
		uint8_t time;
		vector_t accel;
		vector_t gyro;
		double data[3];
		vector_t angle_mv;
		
		for (uint8_t i = 0; i < 1000; i++) {
			time = timer_millis();
			accel = chiindii->getMpu6050()->getAccel();
			gyro = chiindii->getMpu6050()->getGyro();

			// compute the absolute angle relative to the horizontal
			angle_mv.x = atan2(accel.z, accel.x);
			angle_mv.y = atan2(accel.z, accel.y);
		
			// complementary tuning
			// filter gyro rate and measured angle increase the accuracy of the angle
			chiindii->getCompX()->compute(gyro.x, angle_mv.x, &angle_mv.x, time);
			chiindii->getCompX()->compute(gyro.y, angle_mv.y, &angle_mv.y, time);
			
			FramedSerialMessage response(MESSAGE_SEND_TUNING_DATA, (uint8_t*) data, 12);
			chiindii->sendMessage(&response);

			_delay_ms(10);
		}
	}	
}
