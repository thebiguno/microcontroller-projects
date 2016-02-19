#include "Calibration.h"

#include <avr/wdt.h>

#include "../Chiindii.h"
#include "../timer/timer.h"

using namespace digitalcave;

#ifdef DEBUG
char temp[128];
#endif

//1 byte read from EEPROM_MAGIC must be 0x42; otherwise we assume eeprom is invalid.
/* EEPROM format.  Offsets read from EEPROM_OFFSET.  

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
60 g-force z p
64 g-force z i
68 g-force z d
72 comp x t
76 comp y t

 */

Calibration::Calibration(Chiindii* chiindii) {
	this->chiindii = chiindii;
}

void Calibration::read() {
	if (eeprom_read_byte((uint8_t*) EEPROM_MAGIC) == 0x42){
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
		chiindii->getAngleY()->setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) EEPROM_OFFSET + 60);
		ki = eeprom_read_float((float*) EEPROM_OFFSET + 64);
		kd = eeprom_read_float((float*) EEPROM_OFFSET + 68);
		chiindii->getGforce()->setTunings(kp, ki, kd);

		t = eeprom_read_float((float*) EEPROM_OFFSET + 72);
		chiindii->getCompX()->setTau(t);
		t = eeprom_read_float((float*) EEPROM_OFFSET + 76);
		chiindii->getCompY()->setTau(t);
		
		//6 * 2 bytes = 12 bytes total for accel + gyro calibration
		int16_t calibration[3];
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 80), 6);
		chiindii->getMpu6050()->setAccelCalib(calibration);
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 86), 6);
		chiindii->getMpu6050()->setGyroCalib(calibration);
#ifdef DEBUG
		chiindii->sendUsb("Load EEPROM\n");
#endif
	}
	else {
#ifdef DEBUG
		chiindii->sendUsb("Load Defaults\n");
#endif
	}
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

	PID* gforce = chiindii->getGforce();
	eeprom_update_float((float*) EEPROM_OFFSET + 60, gforce->getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 64, gforce->getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 68, gforce->getKd());

	Complementary* c_x = chiindii->getCompX();
	eeprom_update_float((float*) EEPROM_OFFSET + 72, c_x->getTau());
	
	Complementary* c_y = chiindii->getCompY();
	eeprom_update_float((float*) EEPROM_OFFSET + 76, c_y->getTau());
	
	eeprom_update_block(chiindii->getMpu6050()->getAccelCalib(), (void*) (EEPROM_OFFSET + 80), 6);
	eeprom_update_block(chiindii->getMpu6050()->getGyroCalib(), (void*) (EEPROM_OFFSET + 86), 6);
	
	//Write the magic value to say that we have written valid bytes
	eeprom_update_byte((uint8_t*) EEPROM_MAGIC, 0x42);
	
#ifdef DEBUG
	chiindii->sendUsb("Save EEPROM\n");
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
		
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Request Rate PID:\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
		chiindii->sendUsb(temp);
#endif
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_ANGLE_PID){
		PID* x = chiindii->getAngleX();
		PID* y = chiindii->getAngleY();
		PID* g = chiindii->getGforce();
		double data[] = { 
			x->getKp(), x->getKi(), x->getKd(),
			y->getKp(), y->getKi(), y->getKd(),
			g->getKp(), g->getKi(), g->getKd()
		};
		FramedSerialMessage response(MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, (uint8_t*) data, 36);
		chiindii->sendMessage(&response);
		
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Request Angle PID:\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
		chiindii->sendUsb(temp);
#endif
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY){
		Complementary* x = chiindii->getCompX();
		Complementary* y = chiindii->getCompY();
		double data[] = { 
			x->getTau(), y->getTau()
		};
		FramedSerialMessage response(MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY, (uint8_t*) data, 8);
		chiindii->sendMessage(&response);
		
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Calibration requested comp: %f, %f\n", x->getTau(), y->getTau());
		chiindii->sendUsb(temp);
#endif
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_RATE_PID){
		double* data = (double*) request->getData();
		chiindii->getRateX()->setTunings(data[0], data[1], data[2]);
		chiindii->getRateY()->setTunings(data[3], data[4], data[5]);
		chiindii->getRateZ()->setTunings(data[6], data[7], data[8]);
		
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Set Rate PID:\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
		chiindii->sendUsb(temp);
#endif

	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_ANGLE_PID){
		double* data = (double*) request->getData();
		chiindii->getAngleX()->setTunings(data[0], data[1], data[2]);
		chiindii->getAngleY()->setTunings(data[3], data[4], data[5]);
		chiindii->getGforce()->setTunings(data[6], data[7], data[8]);
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Set Angle PID:\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n%3.2f, %3.2f, %3.2f\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
		chiindii->sendUsb(temp);
#endif
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_COMPLEMENTARY){
		double* data = (double*) request->getData();
		chiindii->getCompX()->setTau(data[0]);
		chiindii->getCompY()->setTau(data[1]);
		
#ifdef DEBUG
		snprintf(temp, sizeof(temp), "Calibration sent comp: %f, %f\n", chiindii->getCompX()->getTau(), chiindii->getCompY()->getTau());
		chiindii->sendUsb(temp);
#endif
	}
	else if (cmd == MESSAGE_START_CALIBRATION_COMPLEMENTARY){
		uint8_t* data = (uint8_t*) request->getData();
		uint8_t axis = data[0];
		
		uint8_t time;
		vector_t accel;
		vector_t gyro;
		double outdata[3];
		vector_t angle_mv;
		
		for (uint16_t i = 0; i < 1000; i++) {
			wdt_reset();
			
			time = timer_millis();
			accel = chiindii->getMpu6050()->getAccel();
			gyro = chiindii->getMpu6050()->getGyro();
			
			if (axis == 0) {
				angle_mv.x = M_PI_2 - atan2(accel.z, accel.x);
				chiindii->getCompX()->compute(gyro.x, angle_mv.x, &angle_mv.x, time);
				outdata[0] = gyro.x;
				outdata[1] = accel.x;
				outdata[2] = angle_mv.x;
			}
			else {
				angle_mv.y = M_PI_2 - atan2(accel.z, accel.y);
				chiindii->getCompX()->compute(gyro.y, angle_mv.y, &angle_mv.y, time);
				outdata[0] = gyro.y;
				outdata[1] = accel.y;
				outdata[2] = angle_mv.y;
			}
			FramedSerialMessage response(MESSAGE_SEND_TUNING_DATA, (uint8_t*) outdata, 12);
			chiindii->sendMessage(&response);

			// TODO this delay should be the about same as the duration of the normal main loop
			_delay_ms(10);
		}
	}
	else if (cmd == MESSAGE_START_MPU_CALIBRATION){
		wdt_enable(WDTO_4S);	//This takes a bit of time... we need to make sure the WDT doesn't reset.
		chiindii->getMpu6050()->calibrate();
		wdt_enable(WDTO_120MS);
	}
}
