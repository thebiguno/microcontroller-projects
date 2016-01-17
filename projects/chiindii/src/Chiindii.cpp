#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <PID.h>
#include <SerialAVR.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "lib/timer/timer.h"

#include "Chiindii.h"

#include "battery/battery.h"
#include "motor/motor.h"

using namespace digitalcave;

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	timer_init();

	Chiindii chiindii;
	chiindii.run();
}

vector_t* Chiindii::getAngleSp() { return &angle_sp; }
vector_t* Chiindii::getRateSp() { return &rate_sp; }
PID* Chiindii::getRateX() { return &rate_x; }
PID* Chiindii::getRateY() { return &rate_y; }
PID* Chiindii::getRateZ() { return &rate_z; }
PID* Chiindii::getAngleX() { return &angle_x; }
PID* Chiindii::getAngleY() { return &angle_y; }
Complementary* Chiindii::getCompX() { return &c_x; }
Complementary* Chiindii::getCompY() { return &c_y; }
Mpu6050* Chiindii::getMpu6050() { return &mpu6050; }
uint8_t Chiindii::getBatteryLevel() { return battery_level; }
uint8_t Chiindii::getMode() { return mode; }
void Chiindii::setMode(uint8_t mode) { this->mode = mode; }

void Chiindii::setThrottle(double throttle) { this->throttle = throttle; }

Chiindii::Chiindii() : 
	serial(32400),
	protocol(40),
	
	rate_x(1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_y(1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_z(1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
	angle_x(1, 0, 0, DIRECTION_NORMAL, 10, 0),
	angle_y(1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
	c_x(0.075, 10, 0),
	c_y(0.075, 10, 0),
	
	general(this),
	calibration(this),
	direct(this),
	uc(this)
{
	throttle = 0;
	mode = MODE_UNARMED;
}

void Chiindii::run() {
	FramedSerialMessage request(0,40);
	
	calibration.read(); // load PID and comp tuning values from EEPROM
	
	vector_t gyro;
	vector_t accel;
	vector_t rate_pv;
	vector_t angle_mv;
	uint32_t time = 0;
	uint32_t last_message_time = 0;
	
	motor_start();
	
	//Main program loop
	while (1) {
		time = timer_millis();

		if (protocol.read(&serial, &request)) {
			dispatch(&request);
			last_message_time = time;
			status.commOK();
		} else if (time - last_message_time > 2000) { // TODO is 2 seconds OK?
			mode = MODE_UNARMED;
			status.commInterrupt();
		}
		
		battery_level = battery_read();
		if (battery_level > BATTERY_WARNING_LEVEL) {
			status.batteryOK();
		} else if (battery_level > BATTERY_DAMAGE_LEVEL) {
			status.batteryLow();
		} else {
			mode = MODE_UNARMED;
			status.batteryLow();
		}

		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		// compute the absolute angle relative to the horizontal
		angle_mv.x = atan2(accel.z, accel.x);
		angle_mv.y = atan2(accel.z, accel.y);
		// NOTE can't do this for Z axis without a magnetometer
		
		// complementary tuning
		// filter gyro rate and measured angle increase the accuracy of the angle
		c_x.compute(gyro.x, angle_mv.x, &angle_mv.x, time);
		c_y.compute(gyro.y, angle_mv.y, &angle_mv.y, time);

		if (mode == MODE_ARMED_ANGLE) {
			// angle pid
			// compute a rate set point given an angle set point and current measured angle
			angle_x.compute(angle_sp.x, angle_mv.x, &rate_sp.x, time);
			angle_y.compute(angle_sp.y, angle_mv.y, &rate_sp.y, time);
		}
		
		// rate pid
		// computes the desired change rate
		rate_x.compute(rate_sp.x, gyro.x, &rate_pv.x, time);
		rate_y.compute(rate_sp.y, gyro.y, &rate_pv.y, time);
		rate_z.compute(rate_sp.z, gyro.z, &rate_pv.z, time);
		
		if (mode == MODE_ARMED_RATE || mode == MODE_ARMED_ANGLE) {
			status.armed();
			driveMotors(rate_pv);
		} else {
			status.unarmed();
		}

		status.poll(time);

		// TODO what is the purpose of having a delay here?
		_delay_ms(10);
	}
}

void Chiindii::driveMotors(vector_t rate_pv) {
	double m1 = throttle - rate_pv.x - rate_pv.y - rate_pv.z;
	double m2 = throttle + rate_pv.x - rate_pv.y + rate_pv.z;
	double m3 = throttle + rate_pv.x + rate_pv.y - rate_pv.z;
	double m4 = throttle - rate_pv.x + rate_pv.y + rate_pv.z;

	motor_set(m1, m2, m3, m4);
} 

void Chiindii::dispatch(FramedSerialMessage *message) {
	uint8_t cmd = message->getCommand();

	if (cmd == MESSAGE_REQUEST_ENABLE_DEBUG) {
		debug = 0x01;
		FramedSerialMessage msg(MESSAGE_REQUEST_ENABLE_DEBUG, 0);
		protocol.write(&serial, &msg);
	}
	else if (cmd == MESSAGE_REQUEST_DISABLE_DEBUG) {
		debug = 0x00;
		FramedSerialMessage msg(MESSAGE_REQUEST_DISABLE_DEBUG, 0);
		protocol.write(&serial, &msg);
	}
	//This is a Universal Controller message (namespace 0x1X)
	else if ((cmd & 0xF0) == 0x10){
		uc.dispatch(message);
	}
	//This is a Direct API message (namespace 0x2X)
	else if ((cmd & 0xF0) == 0x20){
		direct.dispatch(message);
	}
	//This is a Calibration API message (namespace 0x3X)
	else if ( (cmd & 0xF0) == 0x30){
		calibration.dispatch(message);
	}
	else {
		//TODO Send debug message 'unknown command' or similar
	}
}

void Chiindii::sendMessage(FramedSerialMessage *message) {
	protocol.write(&serial, message);
}
