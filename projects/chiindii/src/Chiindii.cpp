#include "Chiindii.h"

#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>

#include <PID.h>
#include <SerialAVR.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "timer/timer.h"

#include "battery/battery.h"
#include "motor/motor.h"

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialAVR serial(38400, 8, 0, 1, 1, 64);
#ifdef DEBUG
SerialUSB usb;
#endif

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	battery_init();
	timer_init();

/*
	motor_start();
	while (1) {
	for (uint16_t i = 0; i < 512; i++) {
			motor_set(i,i,i,i);
			_delay_ms(10);
		}
	}
*/

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
PID* Chiindii::getGforce() { return &gforce; }
Complementary* Chiindii::getCompX() { return &c_x; }
Complementary* Chiindii::getCompY() { return &c_y; }
Mpu6050* Chiindii::getMpu6050() { return &mpu6050; }
uint8_t Chiindii::getBatteryLevel() { return battery_level; }
uint8_t Chiindii::getBatteryPercent() { return battery_pct(); }
uint8_t Chiindii::getMode() { return mode; }
void Chiindii::setMode(uint8_t mode) { this->mode = mode; }
uint8_t Chiindii::getDebug() { return debug; }
void Chiindii::setDebug(uint8_t debug) { this->debug = debug; }

void Chiindii::sendDebug(const char* message){
	sendDebug((char*) message);
}
void Chiindii::sendDebug(char* message){
	if (debug){
		FramedSerialMessage response(MESSAGE_DEBUG, (uint8_t*) message, strnlen(message, 128));
		sendMessage(&response);
	}
}

void Chiindii::setThrottle(double throttle) { 
	if (throttle < 0) throttle_sp = 0; 
	else if (throttle > 1) throttle_sp = 1; 
	this->throttle_sp = throttle; 
}

Chiindii::Chiindii() : 
	mode(MODE_UNARMED),
	debug(1),			//TODO We currently start in debug mode
	throttle_sp(0),
	angle_sp({0, 0, 0}),
	rate_sp({0, 0, 0}),
	protocol(40),
	
	rate_x(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_y(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_z(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
	angle_x(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	angle_y(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	gforce(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
	c_x(0.075, 3, 0),
	c_y(0.075, 3, 0),
	
	general(this),
	calibration(this),
	direct(this),
	uc(this)
{
	//Output of angle PID is a rate (rad / s) for each axis.  -1 to 1 should suffice; if this
	// results in too slow corrections, we can increase.
	angle_x.setOutputLimits(-1, 1);
	angle_y.setOutputLimits(-1, 1);
	
	//Output of g-force PID
	gforce.setOutputLimits(0.9, 1.1);

	//Output of rate PID is WHAT?!?!?!?
	rate_x.setOutputLimits(-1, 1);
	rate_y.setOutputLimits(-1, 1);
	rate_z.setOutputLimits(0, 1);
}

void Chiindii::run() {
	FramedSerialMessage request(0,40);

	calibration.read(); // load previously saved PID and comp tuning values from EEPROM
	
	vector_t gyro;
	vector_t accel;
	vector_t rate_pv;
	vector_t angle_mv;
	uint32_t time = 0;
	uint32_t last_message_time = 0;
	
	motor_start();
	
	_delay_ms(250);
/*	
	motor_set(64,0,0,0);
	_delay_ms(250);
	motor_set(0,64,0,0);
	_delay_ms(250);
	motor_set(0,0,64,0);
	_delay_ms(250);
	motor_set(0,0,0,64);
	_delay_ms(250);
	motor_set(0,0,0,0);
*/
	//Watchdog timer
	wdt_enable(WDTO_120MS);
	
	//Main program loop
	while (1) {
		wdt_reset();
		time = timer_millis();
		
		if (protocol.read(&serial, &request)) {
			dispatch(&request);
			last_message_time = time;
			status.commOK();
		} else if (time - last_message_time > 1000) {
			if (mode) sendDebug("Comm Timeout");
			mode = MODE_UNARMED;
			status.commInterrupt();
		}

		battery_level = battery_read();
		if (battery_level > BATTERY_WARNING_LEVEL) {
			status.batteryOK();
		} else if (battery_level > BATTERY_DAMAGE_LEVEL) {
			status.batteryLow();
		} else if (battery_level <= 1) {
			//The battery should only read as 0 if it is unplugged; we assume that we 
			// are running in debug mode without any battery.  We still show the battery 
			// status light, but we don't exit from armed mode.
			status.batteryLow();
		} else {
			if (mode) sendDebug("Low Battery");
			mode = MODE_UNARMED;
			status.batteryLow();
		}

		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		// compute the absolute angle relative to the horizontal
		angle_mv.x = M_PI_2 - atan2(accel.z, accel.x);
		angle_mv.y = M_PI_2 - atan2(accel.z, accel.y);

		// complementary tuning
		// filter gyro rate and measured angle increase the accuracy of the angle
		uint8_t computed = 0;
		computed |= c_x.compute(gyro.x, angle_mv.x, &angle_mv.x, time);
		computed |= c_y.compute(gyro.y, angle_mv.y, &angle_mv.y, time);
		
		if (computed){
			if (mode == MODE_ARMED_ANGLE) {
				// angle pid
				// compute a rate set point given an angle set point and current measured angle
				angle_x.compute(angle_sp.x, angle_mv.x, &rate_sp.x, time);
				angle_y.compute(angle_sp.y, angle_mv.y, &rate_sp.y, time);
				gforce.compute(angle_sp.z, angle_mv.z, &throttle_sp, time);
			}
			else if (mode == MODE_ARMED_RATE){
				// rate pid
				// computes the desired change rate
				rate_x.compute(rate_sp.x, gyro.x, &rate_pv.x, time);
				rate_y.compute(rate_sp.y, gyro.y, &rate_pv.y, time);
				rate_z.compute(rate_sp.z, gyro.z, &rate_pv.z, time);
			}
			else {
				//If we are not armed, keep the PID reset.  This prevents erratic behaviour 
				// when initially turning on, especially if I is non-zero.
				rate_x.reset(time);
				rate_y.reset(time);
				rate_z.reset(time);
				angle_x.reset(time);
				angle_y.reset(time);
				gforce.reset(time);
			}
			
			if (mode == MODE_ARMED_RATE || mode == MODE_ARMED_ANGLE) {
				status.armed();

				//Debug data
				/*
				if (debug){
					char temp[128];
					snprintf(temp, sizeof(temp), "Attitude: %3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f", gyro.x, gyro.y, accel.x, accel.y, angle_mv.x, angle_mv.y);
					sendDebug(temp);
					snprintf(temp, sizeof(temp), "PID: %3.2f,%3.2f,%3.2f", rate_pv.x, rate_pv.y, rate_pv.z);
					sendDebug(temp);
				}
				*/

				driveMotors(&rate_pv);
			} else {
				status.disarmed();
				motor_set(0, 0, 0, 0);
			}
		}

		status.poll(time);
	}
}

void Chiindii::driveMotors(vector_t* rate_pv) {
	double m1 = throttle_sp - rate_pv->x - rate_pv->y - rate_pv->z;
	double m2 = throttle_sp + rate_pv->x - rate_pv->y + rate_pv->z;
	double m3 = throttle_sp + rate_pv->x + rate_pv->y - rate_pv->z;
	double m4 = throttle_sp - rate_pv->x + rate_pv->y + rate_pv->z;

	/*
	if (debug){
		char temp[128];
		snprintf(temp, sizeof(temp), "Raw Motors: %3.2f, %3.2f, %3.2f, %3.2f", m1, m2, m3, m4);
		sendDebug(temp);
	}
	*/
	
	//We limit the motor outputs to be in the range [0, 1].
	if (m1 < 0) m1 = 0;
	else if (m1 > 1) m1 = 1;
	if (m2 < 0) m2 = 0;
	else if (m2 > 1) m2 = 1;
	if (m3 < 0) m3 = 0;
	else if (m3 > 1) m3 = 1;
	if (m4 < 0) m4 = 0;
	else if (m4 > 1) m4 = 1;
	
	//Convert values in [0..1] to [0..511] (9 bit motor control)
	m1 = m1 * 511;
	m2 = m2 * 511;
	m3 = m3 * 511;
	m4 = m4 * 511;
	
	/*
	if (debug){
		char temp[128];
		snprintf(temp, sizeof(temp), "Motors: %d, %d, %d, %d", (uint16_t) m1, (uint16_t) m2, (uint16_t) m3, (uint16_t) m4);
		sendDebug(temp);
	}
	*/

	motor_set(m1, m2, m3, m4);
} 

void Chiindii::dispatch(FramedSerialMessage* request) {
	uint8_t cmd = request->getCommand();

	if ((cmd & 0xF0) == 0x00){
		general.dispatch(request);
	}
	else if ((cmd & 0xF0) == 0x10){
		uc.dispatch(request);
	}
	else if ((cmd & 0xF0) == 0x20){
		direct.dispatch(request);
	}
	else if ( (cmd & 0xF0) == 0x30){
		calibration.dispatch(request);
	}
	else {
		//TODO Send debug message 'unknown command' or similar
	}
}

void Chiindii::sendMessage(FramedSerialMessage* message) {
	protocol.write(&serial, message);
}

ISR(USART1_RX_vect){
	serial.isr();
}