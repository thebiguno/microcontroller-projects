#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <PID.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "lib/timer/timer.h"

#include "Chiindii.h"
#include "Complementary.h"
#include "Status.h"

#include "battery/battery.h"
#include "motor/motor.h"

using namespace digitalcave;

void drive_motors(double throttle, vector_t rate);

double throttle = 0;
vector_t rate_sp;
vector_t angle_sp;

uint8_t mode = MODE_UNARMED;
uint8_t controller;

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	timer_init();
	
	Mpu6050 mpu6050;
//	mpu6050.calibrate();

	Status status;

	// TODO supply proper kp, ki, kd
	PID rate_x(1, 0, 0, DIRECTION_NORMAL, 10, 0); // 10 = 10 ms
	PID rate_y(1, 0, 0, DIRECTION_NORMAL, 10, 0);
	PID rate_z(1, 0, 0, DIRECTION_NORMAL, 10, 0);
	
	PID angle_x(1, 0, 0, DIRECTION_NORMAL, 10, 0); // 10 = 10 ms
	PID angle_y(1, 0, 0, DIRECTION_NORMAL, 10, 0);
	
	// TODO supply proper tau
	Complementary c_x(0.075, 10, 0); // 10 = 10 ms
	Complementary c_y(0.075, 10, 0);
	
	vector_t gyro;
	vector_t accel;
	vector_t rate_pv;
	vector_t angle_mv;
	uint32_t time;
	
	motor_start();
	
	//Main program loop
	while (1) {
		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();
		time = timer_millis();

		// compute the absolute angle relative to the horizontal
		angle_mv.x = atan2(accel.z, accel.x);
		angle_mv.y = atan2(accel.z, accel.y);
		// NOTE can't do this for Z axis without a magnetometer
		
		// complementary tuning
		// filter gyro rate and measured angle increase the accuracy of the angle
		c_x.compute(gyro.x, angle_mv.x, &angle_mv.x, time);
		c_y.compute(gyro.y, angle_mv.y, &angle_mv.y, time);

		if (mode == MODE_ARMED_ANGLE) {
			angle_sp.x = 0; // TODO get this from the controller
			angle_sp.y = 0;

			// angle pid
			// compute a rate set point given an angle set point and current measured angle
			angle_x.compute(angle_sp.x, angle_mv.x, &rate_sp.x, time);
			angle_y.compute(angle_sp.y, angle_mv.y, &rate_sp.y, time);
		} else {
			rate_sp.x = 0; // TODO get this from the controller
			rate_sp.y = 0;
		}
		
		rate_sp.z = 0; // TODO get this from the controller

		// rate pid
		// computes the desired change rate
		rate_x.compute(rate_sp.x, gyro.x, &rate_pv.x, time);
		rate_y.compute(rate_sp.y, gyro.y, &rate_pv.y, time);
		rate_z.compute(rate_sp.z, gyro.z, &rate_pv.z, time);
		
		uint8_t battery_level = battery_read();
		if (battery_level > BATTERY_WARNING_LEVEL) {
			status.batteryOK();
		} else {
			status.batteryLow();
		}
		
		if (mode == MODE_ARMED_RATE || mode == MODE_ARMED_ANGLE) {
			status.armed();
			if (battery_level > BATTERY_DAMAGE_LEVEL) {
				drive_motors(throttle, rate_pv);
			} else {
				motor_set(0, 0, 0, 0);
			}
		} else {
			status.unarmed();
		}

		status.poll(time);

		_delay_ms(10);
	}
}

void drive_motors(double throttle, vector_t rate_pv) {
	double m1 = throttle - rate_pv.x - rate_pv.y - rate_pv.z;
	double m2 = throttle + rate_pv.x - rate_pv.y + rate_pv.z;
	double m3 = throttle + rate_pv.x + rate_pv.y - rate_pv.z;
	double m4 = throttle - rate_pv.x + rate_pv.y + rate_pv.z;

	motor_set(m1, m2, m3, m4);
} 



/*
 * Called from ISR; keep things as short as possible.
 */
void dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_ANNOUNCE_CONTROL_ID){
		if (message[0] == 'U'){
			controller = CONTROLLER_UC;
		}
		else if (message[0] == 'D'){
			controller = CONTROLLER_DIRECT;
		}
		else if (message[0] == 'C'){
			controller = CONTROLLER_CALIBRATION;
		}
		//TODO Put any other supported control modes here.
		else {
			controller = CONTROLLER_NONE;
		}
	}
	else if (cmd == MESSAGE_REQUEST_ENABLE_DEBUG){
		debug = 0x01;
		doAcknowledgeCommand(MESSAGE_REQUEST_ENABLE_DEBUG);
	}
	else if (cmd == MESSAGE_REQUEST_DISABLE_DEBUG){
		debug = 0x00;
		doAcknowledgeCommand(MESSAGE_REQUEST_DISABLE_DEBUG);
	}
	//This is a Universal Controller message (namespace 0x1X)
	else if (controller == CONTROLLER_UC && (cmd & 0xF0) == 0x10){
		uc_dispatch_message(cmd, message, length);
	}
	//This is a Processing API message (namespace 0x2X)
	else if (controller == CONTROLLER_PROCESSING && (cmd & 0xF0) == 0x20){
		processing_dispatch_message(cmd, message, length);
	}
	//This is a Calibration API message (namespace 0x3X)
	else if (controller == CONTROLLER_CALIBRATION && (cmd & 0xF0) == 0x30){
		calibration_dispatch_message(cmd, message, length);
	}
	else {
		//TODO Send debug message 'unknown command' or similar
	}
}
