#include "Chiindii.h"

#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>

#include <PID.h>
#include <SerialAVR.h>

#include "timer/timer.h"

#include "battery/battery.h"
#include "motor/motor.h"

//The period (in ms) since we last saw a message, after which we assume the comm link is dead and we disarm the craft
#define COMM_TIMEOUT_PERIOD		1000

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialAVR serial(38400, 8, 0, 1, 1, 128);

//Reset WDT after system reset
void get_mcusr(void) __attribute__((naked))  __attribute__((section(".init3")));
void get_mcusr(void) {
	MCUSR = 0;
	wdt_disable();
}

int main(){
	wdt_disable();
	
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	DDRB |= _BV(PORTB1);
	
	battery_init();
	timer_init();

	Chiindii chiindii;
	chiindii.run();
}

Chiindii::Chiindii() : 
	mode(MODE_UNARMED),
	debug(1),			//TODO We currently start in debug mode
	throttle_sp(0),
	angle_sp({0, 0, 0}),
	rate_sp({0, 0, 0}),
	protocol(128),
	
	rate_x(0.1, 0, 0, DIRECTION_NORMAL, 0),
	rate_y(0.1, 0, 0, DIRECTION_NORMAL, 0),
	rate_z(0.1, 0, 0, DIRECTION_NORMAL, 0),
	
	angle_x(0.1, 0, 0, DIRECTION_NORMAL, 0),
	angle_y(0.1, 0, 0, DIRECTION_NORMAL, 0),
	angle_z(0.1, 0, 0, DIRECTION_NORMAL, 0),
	
	gforce(0.1, 0, 0, DIRECTION_NORMAL, 0),
	
	madgwick(0.01, 0),
	
	general(this),
	calibration(this),
	direct(this),
	uc(this)
{
	//Output of angle PID is a rate (rad / s) for each axis.
	angle_x.setOutputLimits(-10, 10);
	angle_y.setOutputLimits(-10, 10);
	angle_z.setOutputLimits(-1, 1);
	
	//Output of g-force PID
	gforce.setOutputLimits(0, 1);

	//Output of rate PID is an acceleration (rad / s / s) for each axis
	rate_x.setOutputLimits(-4, 4);
	rate_y.setOutputLimits(-4, 4);
	rate_z.setOutputLimits(-1, 1);
}

void Chiindii::run() {
	FramedSerialMessage request(0, 128);

	calibration.read(); // load previously saved PID and comp tuning values from EEPROM
	
	vector_t gyro = {0, 0, 0};
	vector_t accel = {0, 0, 0};
	vector_t rate_pv = {0, 0, 0};
	vector_t angle_mv = {0, 0, 0};
	uint32_t time = 0;
	uint32_t last_message_time = 0;
	
	motor_start();
	
	_delay_ms(250);

	//Watchdog timer
	wdt_enable(WDTO_120MS);
	
	//Main program loop
	while (1) {
		PORTB ^= _BV(PORTB1);
		wdt_reset();
		time = timer_millis();
		
		if (protocol.read(&serial, &request)) {
			uint8_t cmd = request.getCommand();

			if ((cmd & 0xF0) == 0x00){
				general.dispatch(&request);
			}
			else if ((cmd & 0xF0) == 0x10){
				uc.dispatch(&request);
			}
			else if ((cmd & 0xF0) == 0x20){
				direct.dispatch(&request);
			}
			else if ((cmd & 0xF0) == 0x30){
				calibration.dispatch(&request);
			}
			else {
				//TODO Send debug message 'unknown command' or similar
			}
			last_message_time = time;
			status.commOK();
		}
		else if ((time - last_message_time) > COMM_TIMEOUT_PERIOD) {
			if (mode) sendStatus("Comm Timeout  ");
			mode = MODE_UNARMED;
			status.commInterrupt();
		}

		battery_level = battery_read();
		if (battery_level > BATTERY_WARNING_LEVEL) {
			status.batteryOK();
		}
		else if (battery_level > BATTERY_DAMAGE_LEVEL) {
			status.batteryLow();
		}
		else if (battery_level <= 1) {
			//The battery should only read as 0 (or 1) if it is completely unplugged; we assume that we 
			// are running in debug mode without any battery.  We still show the battery 
			// status light, but we don't exit from armed mode.
			status.batteryLow();
		}
		else {
			if (mode) sendStatus("Low Battery  ");
			mode = MODE_UNARMED;
			status.batteryLow();
		}

		//Update IMU calculations.
		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		madgwick.compute(accel, gyro, mode, time);
		
		//Update PID calculations and adjust motors
		angle_mv = madgwick.getEuler();

		double throttle;
		
		//We only do angle PID in mode angle or throttle.
		if (mode == MODE_ARMED_ANGLE) {
			double accel_mv = madgwick.getZAcceleration(accel);
			
			// angle pid
			// compute a rate set point given an angle set point and current measured angle
			// see doc/control_system.txt
			rate_sp.x = angle_x.compute(angle_sp.x, angle_mv.x, time);
			rate_sp.y = angle_y.compute(angle_sp.y, angle_mv.y, time);
			rate_sp.z = angle_z.compute(angle_sp.z, angle_mv.z, time);
			throttle = gforce.compute(angle_sp.z, accel_mv, time);
		}
		else if (mode == MODE_ARMED_THROTTLE) {
			// angle pid with direct throttle
			// compute a rate set point given an angle set point and current measured angle
			// see doc/control_system.txt
			rate_sp.x = angle_x.compute(angle_sp.x, angle_mv.x, time);
			rate_sp.y = angle_y.compute(angle_sp.y, angle_mv.y, time);
			rate_sp.z = angle_z.compute(angle_sp.z, angle_mv.z, time);
			gforce.reset(time);
			
			throttle = throttle_sp;
		}
		else {
			angle_x.reset(time);
			angle_y.reset(time);
			angle_z.reset(time);
			gforce.reset(time);
			
			throttle = throttle_sp;
		}
		
		//We always want to do rate PID when armed; if we are in rate mode, then we use the rate_sp as passed
		// by the user, otherwise we use rate_sp as the output of angle PID.
		if (mode && throttle_sp > 0.01){
			// rate pid
			// computes the desired change rate
			// see doc/control_system.txt
			rate_pv.x = rate_x.compute(rate_sp.x, gyro.x, time);
			rate_pv.y = rate_y.compute(rate_sp.y, gyro.y, time);
			rate_pv.z = rate_z.compute(rate_sp.z, gyro.z, time);
			
			//This is the weight which we give to throttle relative to the rate PID outputs.
			// Keeping this too low will result in not enough throttle control; keeping it too high
			// will result in not enough attitude control.
			const double THROTTLE_WEIGHT = 5;
			throttle = throttle * THROTTLE_WEIGHT;
			
			//This assumes an MPU that has a gyro output corresponding to the notes in doc/motor_arrangement.txt, in X configuration
			double m1 = throttle + rate_pv.x - rate_pv.y + rate_pv.z;
			double m2 = throttle - rate_pv.x - rate_pv.y - rate_pv.z;
			double m3 = throttle - rate_pv.x + rate_pv.y + rate_pv.z;
			double m4 = throttle + rate_pv.x + rate_pv.y - rate_pv.z;

			if (m1 < 0) m1 = 0;
			else if (m1 > THROTTLE_WEIGHT) m1 = THROTTLE_WEIGHT;
			if (m2 < 0) m2 = 0;
			else if (m2 > THROTTLE_WEIGHT) m2 = THROTTLE_WEIGHT;
			if (m3 < 0) m3 = 0;
			else if (m3 > THROTTLE_WEIGHT) m3 = THROTTLE_WEIGHT;
			if (m4 < 0) m4 = 0;
			else if (m4 > THROTTLE_WEIGHT) m4 = THROTTLE_WEIGHT;

			//Convert values in [0..1] to [0..511] (9 bit motor control)
			m1 = m1 * 511 / THROTTLE_WEIGHT;
			m2 = m2 * 511 / THROTTLE_WEIGHT;
			m3 = m3 * 511 / THROTTLE_WEIGHT;
			m4 = m4 * 511 / THROTTLE_WEIGHT;

			motor_set(m1, m2, m3, m4);

			status.armed();
		}
		else {
			//If we are not armed, keep the PID reset.  This prevents erratic behaviour 
			// when initially turning on, especially if I is non-zero.
			rate_x.reset(time);
			rate_y.reset(time);
			rate_z.reset(time);
			angle_x.reset(time);
			angle_y.reset(time);
			angle_z.reset(time);
			gforce.reset(time);
			
			status.disarmed();
			motor_set(0, 0, 0, 0);
		}
			
// #ifdef DEBUG
// 		if (debug){
// 			char temp[128];
// 			if (mode){
// 				snprintf(temp, sizeof(temp), "Gyro: %4d, %4d, %4d  ", (int16_t) radToDeg(gyro.x), (int16_t) radToDeg(gyro.y), (int16_t) radToDeg(gyro.z));
// 				sendDebug(temp);
// 			}
// 			if (mode == MODE_ARMED_THROTTLE || mode == MODE_ARMED_ANGLE || mode == MODE_SHOW_VARIABLES){
// 				snprintf(temp, sizeof(temp), "Accel: %4d, %4d, %4d  ", (int16_t) (accel.x * 100), (int16_t) (accel.y * 100), (int16_t) (accel.z * 100));
// 				sendDebug(temp);
// 				snprintf(temp, sizeof(temp), "Angle MV: %4d, %4d, %4d  ", (int16_t) radToDeg(angle_mv.x), (int16_t) radToDeg(angle_mv.y), (int16_t) radToDeg(angle_mv.z));
// 				sendDebug(temp);
// 				snprintf(temp, sizeof(temp), "Rate SP: %3d, %3d, %3d  ", (int16_t) (rate_sp.x * 100), (int16_t) (rate_sp.y * 100), (int16_t) (rate_sp.z * 100));
// 				sendDebug(temp);
// 			}
// 			if (mode){
// 				snprintf(temp, sizeof(temp), "Rate PV: %3d, %3d, %3d  ", (int16_t) (rate_pv.x * 100), (int16_t) (rate_pv.y * 100), (int16_t) (rate_pv.z * 100));
// 				sendDebug(temp);
// 			}
// 			if (mode == MODE_SHOW_VARIABLES){
// 				sendDebug("\n");
// 			}
// 		}
// #endif

		status.poll(time);
	}
}

vector_t* Chiindii::getAngleSp() { return &angle_sp; }
vector_t* Chiindii::getRateSp() { return &rate_sp; }
PID* Chiindii::getRateX() { return &rate_x; }
PID* Chiindii::getRateY() { return &rate_y; }
PID* Chiindii::getRateZ() { return &rate_z; }
PID* Chiindii::getAngleX() { return &angle_x; }
PID* Chiindii::getAngleY() { return &angle_y; }
PID* Chiindii::getAngleZ() { return &angle_z; }
PID* Chiindii::getGforce() { return &gforce; }
Mpu6050* Chiindii::getMpu6050() { return &mpu6050; }
uint8_t Chiindii::getBatteryLevel() { return battery_level; }
uint8_t Chiindii::getBatteryPercent() { return battery_pct(); }
uint8_t Chiindii::getMode() { return mode; }
void Chiindii::setMode(uint8_t mode) { this->mode = mode; }
uint8_t Chiindii::getDebug() { return debug; }
void Chiindii::setDebug(uint8_t debug) { this->debug = debug; }
Madgwick* Chiindii::getMadgwick() { return &madgwick; }


void Chiindii::sendDebug(const char* message){
	sendDebug((char*) message);
}
void Chiindii::sendDebug(char* message){
	if (debug){
		FramedSerialMessage response(MESSAGE_DEBUG, (uint8_t*) message, strnlen(message, 128));
		sendMessage(&response);
	}
}
void Chiindii::sendStatus(const char* message){
	sendStatus((char*) message);
}
void Chiindii::sendStatus(char* message){
	FramedSerialMessage response(MESSAGE_STATUS, (uint8_t*) message, strnlen(message, 14));
	sendMessage(&response);
}

double Chiindii::getThrottle() { return this->throttle_sp; }
void Chiindii::setThrottle(double throttle) { 
	if (throttle < 0) throttle_sp = 0; 
	else if (throttle > 1) throttle_sp = 1; 
	this->throttle_sp = throttle; 
}

void Chiindii::sendMessage(FramedSerialMessage* message) {
	protocol.write(&serial, message);
}

ISR(USART1_RX_vect){
	serial.isr();
}