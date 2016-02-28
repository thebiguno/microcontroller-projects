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
	angle_z.setOutputLimits(-10, 10);
	
	//Output of g-force PID
	gforce.setOutputLimits(0, 2);

	//Output of rate PID is an acceleration (rad / s / s) for each axis
	rate_x.setOutputLimits(-4, 4);
	rate_y.setOutputLimits(-4, 4);
	rate_z.setOutputLimits(-4, 4);
}

void Chiindii::run() {
	FramedSerialMessage request(0, 128);

	loadConfig(); // load previously saved PID and comp tuning values from EEPROM
	
	vector_t gyro = {0, 0, 0};
	vector_t accel = {0, 0, 0};
	vector_t rate_pv = {0, 0, 0};
	vector_t angle_mv = {0, 0, 0};
	double gyro_z_average = 0;
	uint32_t time = 0;
	uint32_t last_message_time = 0;
	double lowBatteryThrottle = 0;
	
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
			lowBatteryThrottle += 0.0001;
			status.batteryLow();
			if (lowBatteryThrottle > 0.75){
				mode = MODE_UNARMED;
			}
		}

		//Update IMU calculations.
		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		madgwick.compute(accel, gyro, mode, time);
		
		//Update PID calculations and adjust motors
		angle_mv = madgwick.getEuler();

		double throttle;
		
		//We only do angle PID in mode angle or throttle.
		if (mode == MODE_ARMED_ANGLE) { // 0x01
			double accel_mv = madgwick.getZAcceleration(accel);
			
			// angle pid
			// compute a rate set point given an angle set point and current measured angle
			// see doc/control_system.txt
			rate_sp.x = angle_x.compute(angle_sp.x, angle_mv.x, time);
			rate_sp.y = angle_y.compute(angle_sp.y, angle_mv.y, time);
			rate_sp.z = angle_z.compute(angle_sp.z, angle_mv.z, time);
			throttle = gforce.compute(throttle_sp, accel_mv, time);
			//TODO Remove this debugging once we figure out why PID is not adjusting throttle properly...
			char temp[14];
			snprintf(temp, sizeof(temp), "%3d %3d %3d", (int16_t) (throttle_sp * 100), (int16_t) (accel_mv * 100), (int16_t) (throttle * 100));
			sendDebug(temp);
		}
		else if (mode == MODE_ARMED_THROTTLE) { // 0x02
			gyro_z_average = gyro_z_average + gyro.z - (gyro_z_average / 100);
			
			// angle pid with direct throttle
			// compute a rate set point given an angle set point and current measured angle
			// see doc/control_system.txt
			rate_sp.x = angle_x.compute(angle_sp.x, angle_mv.x, time);
			rate_sp.y = angle_y.compute(angle_sp.y, angle_mv.y, time);
			rate_sp.z = angle_z.compute(angle_sp.z, gyro_z_average / 100, time);
//			angle_z.reset(time);
			gforce.reset(time);

// 			char temp[14];
// 			snprintf(temp, sizeof(temp), "%3d %3d %3d", (int16_t) (angle_sp.z * 100), (int16_t) gyro_z_average, (int16_t) (rate_sp.z * 100));
// 			sendDebug(temp);

			throttle = throttle_sp;
		}
		else { // 0x03
			angle_x.reset(time);
			angle_y.reset(time);
			angle_z.reset(time);
			gforce.reset(time);
			
			throttle = throttle_sp;
		}
		
		throttle -= lowBatteryThrottle;
		if (throttle < 0) throttle = 0;
		
		//We always want to do rate PID when armed; if we are in rate mode, then we use the rate_sp as passed
		// by the user, otherwise we use rate_sp as the output of angle PID.
		if (mode){
			// rate pid
			// computes the desired change rate
			// see doc/control_system.txt
			rate_pv.x = rate_x.compute(rate_sp.x, gyro.x, time);
			rate_pv.y = rate_y.compute(rate_sp.y, gyro.y, time);
			rate_pv.z = rate_z.compute(rate_sp.z, gyro.z, time);
			
			//This is the weight which we give to throttle relative to the rate PID outputs.
			// Keeping this too low will result in not enough throttle control; keeping it too high
			// will result in not enough attitude control.
			//By making this dynamic, we allow for more throttle control at the bottom of the throttle
			// range, and more manouverability at the middle / top.
			double throttleWeight = fmax(-3.0 * throttle + 2.5, 1);
			throttle = throttle * throttleWeight;
			
			//This assumes an MPU that has a gyro output corresponding to the notes in doc/motor_arrangement.txt, in X configuration
			double m1 = throttle + rate_pv.x - rate_pv.y + rate_pv.z;
			double m2 = throttle - rate_pv.x - rate_pv.y - rate_pv.z;
			double m3 = throttle - rate_pv.x + rate_pv.y + rate_pv.z;
			double m4 = throttle + rate_pv.x + rate_pv.y - rate_pv.z;

			if (m1 < 0) m1 = 0;
			else if (m1 > throttleWeight) m1 = throttleWeight;
			if (m2 < 0) m2 = 0;
			else if (m2 > throttleWeight) m2 = throttleWeight;
			if (m3 < 0) m3 = 0;
			else if (m3 > throttleWeight) m3 = throttleWeight;
			if (m4 < 0) m4 = 0;
			else if (m4 > throttleWeight) m4 = throttleWeight;

			//Convert values in [0..1] to [0..511] (9 bit motor control)
			m1 = m1 * 511 / throttleWeight;
			m2 = m2 * 511 / throttleWeight;
			m3 = m3 * 511 / throttleWeight;
			m4 = m4 * 511 / throttleWeight;

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

void Chiindii::loadConfig(){
	if (eeprom_read_byte((uint8_t*) EEPROM_MAGIC) == 0x42){
		double kp, ki, kd;
		double t;
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 0));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 4));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 8));
		rate_x.setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 12));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 16));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 20));
		rate_y.setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 24));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 28));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 32));
		rate_z.setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 36));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 40));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 44));
		angle_x.setTunings(kp, ki, kd);

		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 48));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 52));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 56));
		angle_y.setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 60));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 64));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 68));
		angle_z.setTunings(kp, ki, kd);
	
		kp = eeprom_read_float((float*) (EEPROM_OFFSET + 72));
		ki = eeprom_read_float((float*) (EEPROM_OFFSET + 76));
		kd = eeprom_read_float((float*) (EEPROM_OFFSET + 80));
		gforce.setTunings(kp, ki, kd);

		t = eeprom_read_float((float*) (EEPROM_OFFSET + 84));
		madgwick.setBeta(t);
		
		//6 * 2 bytes = 12 bytes total for accel + gyro calibration
		int16_t calibration[3];
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 88), 6);
		mpu6050.setAccelCalib(calibration);
		eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 94), 6);
		mpu6050.setGyroCalib(calibration);
		sendStatus("Load EEPROM   ");
	}
	else {
		sendStatus("Load Defaults ");
	}
}

void Chiindii::saveConfig(){
	cli();
	wdt_disable();
	
	eeprom_update_float((float*) (EEPROM_OFFSET + 0), rate_x.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 4), rate_x.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 8), rate_x.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 12), rate_y.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 16), rate_y.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 20), rate_y.getKd());
	
	eeprom_update_float((float*) (EEPROM_OFFSET + 24), rate_z.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 28), rate_z.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 32), rate_z.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 36), angle_x.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 40), angle_x.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 44), angle_x.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 48), angle_y.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 52), angle_y.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 56), angle_y.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 60), angle_z.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 64), angle_z.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 68), angle_z.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 72), gforce.getKp());
	eeprom_update_float((float*) (EEPROM_OFFSET + 76), gforce.getKi());
	eeprom_update_float((float*) (EEPROM_OFFSET + 80), gforce.getKd());

	eeprom_update_float((float*) (EEPROM_OFFSET + 84), madgwick.getBeta());
	
	eeprom_update_block(mpu6050.getAccelCalib(), (void*) (EEPROM_OFFSET + 88), 6);
	eeprom_update_block(mpu6050.getGyroCalib(), (void*) (EEPROM_OFFSET + 94), 6);
	
	//Write the magic value to say that we have written valid bytes
	eeprom_update_byte((uint8_t*) EEPROM_MAGIC, 0x42);
	
	wdt_enable(WDTO_120MS);
	sei();
	
	sendStatus("Save EEPROM   ");
}

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