#include "Calibration.h"

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

Calibration::Calibration(PID *rate_x, PID *rate_y, PID *rate_z, PID *angle_x, PID *angle_y, Complementary *c_x, Complementary *c_y){
}

void Calibration::read() {
	double kp, ki, kd;
	double t;
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 0);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 4);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 8);
	rate_x.setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 12);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 16);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 20);
	rate_y.setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 24);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 28);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 32);
	rate_z.setTunings(kp, ki, kd);
	
	kp = eeprom_read_float((float*) EEPROM_OFFSET + 36);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 40);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 44);
	angle_x.setTunings(kp, ki, kd);

	kp = eeprom_read_float((float*) EEPROM_OFFSET + 48);
	ki = eeprom_read_float((float*) EEPROM_OFFSET + 52);
	kd = eeprom_read_float((float*) EEPROM_OFFSET + 56);
	angle_y.setTunings(kp, ki, kd);
	
	t = eeprom_read_float((float*) EEPROM_OFFSET + 60);
	c_x.setTunings(t);
	t = eeprom_read_float((float*) EEPROM_OFFSET + 64);
	c_y.setTuning(t);
}

void Calibration::write() {
	eeprom_update_float((float*) EEPROM_OFFSET + 0, rate_x.getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 4, rate_x.getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 8, rate_x.getKd());
	
	eeprom_update_float((float*) EEPROM_OFFSET + 12, rate_y.getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 16, rate_y.getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 20, rate_y.getKd());
	
	eeprom_update_float((float*) EEPROM_OFFSET + 24, rate_z.getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 28, rate_z.getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 32, rate_z.getKd());

	eeprom_update_float((float*) EEPROM_OFFSET + 36, angle_x.getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 40, angle_x.getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 44, angle_x.getKd());

	eeprom_update_float((float*) EEPROM_OFFSET + 48, angle_y.getKp());
	eeprom_update_float((float*) EEPROM_OFFSET + 52, angle_y.getKi());
	eeprom_update_float((float*) EEPROM_OFFSET + 56, angle_y.getKd());

	eeprom_update_float((float*) EEPROM_OFFSET + 60, c_x.getTau());
	eeprom_update_float((float*) EEPROM_OFFSET + 64, c_y.getTau());
}

void Calibration::dispatch(FramedSerialMessage* message) {
	uint8_t cmd = message.getCommand();
	
	if (cmd == MESSAGE_SAVE_CALIBRATION){
		this->write();
	}
	else if (cmd == MESSAGE_LOAD_CALIBRATION){
		this->read();
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_RATE_PID){
		double[] data = { 
			rate_x.getKp(), rate_x.getKi(), rate_x.getKd(),
			rate_y.getKp(), rate_y.getKi(), rate_y.getKd(),
			rate_z.getKp(), rate_z.getKi(), rate_z.getKd()
		};
		FramedSerialMessage message(MESSAGE_REQUEST_CALIBRATION_RATE_PID, (uint8_t*) data, 36);
		message.write();
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_ANGLE_PID){
		double[] data = { 
			angle_x.getKp(), angle_x.getKi(), angle_x.getKd(),
			angle_y.getKp(), angle_y.getKi(), angle_y.getKd()
		};
		FramedSerialMessage message(MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, (uint8_t*) data, 24);
		message.write();
	}
	else if (cmd == MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY){
		double[] data = { 
			c_x.getTau(), c_y.getTau()
		};
		FramedSerialMessage message(MESSAGE_REQUEST_CALIBRATION_RATE_PID, (uint8_t*) data, 8);
		message.write();
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_RATE_PID){
		double[] data = (double[]) message.getData();
		rate_x.setTunings(data[0], data[1], data[2]);
		rate_y.setTunings(data[3], data[4], data[5]);
		rate_z.setTunings(data[6], data[7], data[8]);
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_RATE_PID){
		double[] data = (double[]) message.getData();
		angle_x.setTunings(data[0], data[1], data[2]);
		angle_y.setTunings(data[3], data[4], data[5]);
	}
	else if (cmd == MESSAGE_SEND_CALIBRATION_COMPLEMENTARY){
		double[] data = (double[]) message.getData();
		c_x.setTunings(data[0]);
		c_y.setTunings(data[1]);
	}
	else if (cmd == MESSAGE_START_CALIBRATION_COMPLEMENTARY){
		uint8_t time;
		vector_t accel;
		vector_t gyro;
		float[] data;
		
		for (uint8_t i = 0; i < 1000; i++) {
			time = timer_millis();
			accel = mpu6050.getAccel();
			gyro = mpu6050.getGyro();

			if (x
			// compute the absolute angle relative to the horizontal
			angle_mv.x = atan2(accel.z, accel.x);
			angle_mv.y = atan2(accel.z, accel.y);
		
			// complementary tuning
			// filter gyro rate and measured angle increase the accuracy of the angle
			c_x.compute(gyro.x, angle_mv.x, &angle_mv.x, time);
			c_y.compute(gyro.y, angle_mv.y, &angle_mv.y, time);
			
			data
			
			FramedSerialMessage message(MESSAGE_SEND_TUNING_DATA, (uint8_t*) data, 12)

			_delay_ms(10);
		}
	}	
}
