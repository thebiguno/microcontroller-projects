#include "Chiindii.h"

#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <PID.h>
#include <SerialAVR.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "timer/timer.h"

#include "battery/battery.h"
#include "motor/motor.h"

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialAVR serial(38400, 8, 0, 1, 1, 64);

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	battery_init();
	timer_init();
#ifdef DEBUG
	usb_init();
#endif

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
uint8_t Chiindii::getBatteryPercent() { return battery_pct(); }
uint8_t Chiindii::getMode() { return mode; }
void Chiindii::setMode(uint8_t mode) { this->mode = mode; }
uint8_t Chiindii::getDebug() { return debug; }
void Chiindii::setDebug(uint8_t debug) { this->debug = debug; }

void Chiindii::setThrottle(double throttle) { 
	if (throttle < 0) throttle = 0; 
	else if (throttle > 1) throttle = 1; 
	this->throttle = throttle; 
}

Chiindii::Chiindii() : 
	mode(MODE_UNARMED),
	throttle(0),
	angle_sp({0, 0, 0}),
	rate_sp({0, 0, 0}),
	protocol(40),
	
	rate_x(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_y(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	rate_z(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
	angle_x(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	angle_y(0.1, 0, 0, DIRECTION_NORMAL, 10, 0),
	
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

	//Output of rate PID is a percentage (0-1) for each axis.
	rate_x.setOutputLimits(0, 1);
	rate_y.setOutputLimits(0, 1);
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
	//TODO Remove this, put it in calibration, and save values to EEPROM
	wdt_reset();
	mpu6050.calibrate();
	
	//Main program loop
#ifdef DEBUG
	char temp[128];
#endif

	//Watchdog timer
	wdt_enable(WDTO_120MS);
	wdt_reset();
	
	while (1) {
		wdt_reset();
		time = timer_millis();
		
		if (protocol.read(&serial, &request)) {
#ifdef DEBUG
			uint8_t size = snprintf(temp, sizeof(temp), "Received Command: 0x%02x\n", request.getCommand());
			usb_serial_write((const uint8_t*) temp, size);
#endif
			dispatch(&request);
			last_message_time = time;
			status.commOK();
		} else if (time - last_message_time > 2000) { // TODO is 2 seconds OK?
#ifdef DEBUG
			if (mode) usb_serial_write((const uint8_t*) "Comm timeout\n", 13);
#endif
			mode = MODE_UNARMED;
			status.commInterrupt();
		}

		battery_level = battery_read();
		if (battery_level > BATTERY_WARNING_LEVEL) {
			status.batteryOK();
		} else if (battery_level > BATTERY_DAMAGE_LEVEL) {
			status.batteryLow();
		} else {
#ifdef DEBUG
			if (mode) usb_serial_write((const uint8_t*) "Low battery\n", 12);
#endif
			mode = MODE_UNARMED;
			status.batteryLow();
		}

		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		// compute the absolute angle relative to the horizontal
		angle_mv.x = M_PI_2 - atan2(accel.z, accel.x);
		angle_mv.y = M_PI_2 - atan2(accel.z, accel.y);
		// NOTE can't do this for Z axis without a magnetometer

#ifdef DEBUG
		//uint8_t size = snprintf(temp, sizeof(temp), "%3.5f,%3.5f,", angle_mv.x, gyro.x);
#endif
		
		// complementary tuning
		// filter gyro rate and measured angle increase the accuracy of the angle
		uint8_t computed = 0;
		computed |= c_x.compute(gyro.x, angle_mv.x, &angle_mv.x, time);
		computed |= c_y.compute(gyro.y, angle_mv.y, &angle_mv.y, time);
		
		if (computed){
#ifdef DEBUG
			//usb_serial_write((const uint8_t*) temp, size);
			//size = snprintf(temp, sizeof(temp), "%3.5f\n", angle_mv.x);
			//usb_serial_write((const uint8_t*) temp, size);
#endif

			if (mode == MODE_ARMED_ANGLE) {
				// angle pid
				// compute a rate set point given an angle set point and current measured angle
				angle_x.compute(angle_sp.x, angle_mv.x, &rate_sp.x, time);
				angle_y.compute(angle_sp.y, angle_mv.y, &rate_sp.y, time);
			}

#ifdef DEBUG
			//uint8_t size = snprintf(temp, sizeof(temp), "Rate SP: %3.2f, Gyro MV: %3.2f\n", rate_sp.x, gyro.x);
			//usb_serial_write((const uint8_t*) temp, size);
#endif
			// rate pid
			// computes the desired change rate
			rate_x.compute(rate_sp.x, gyro.x, &rate_pv.x, time);
			rate_y.compute(rate_sp.y, gyro.y, &rate_pv.y, time);
			rate_z.compute(rate_sp.z, gyro.z, &rate_pv.z, time);
		
			if (mode == MODE_ARMED_RATE || mode == MODE_ARMED_ANGLE) {
				status.armed();
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
	double m1 = throttle - rate_pv->x - rate_pv->y - rate_pv->z;
	double m2 = throttle + rate_pv->x - rate_pv->y + rate_pv->z;
	double m3 = throttle + rate_pv->x + rate_pv->y - rate_pv->z;
	double m4 = throttle - rate_pv->x + rate_pv->y + rate_pv->z;

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
	
#ifdef DEBUG
	char temp[128];
	uint8_t size = snprintf(temp, sizeof(temp), "Setting motors: %d, %d, %d, %d from throttle %3.2f and rate_pvs %3.2f, %3.2f, %3.2f\n", (uint16_t) m1, (uint16_t) m2, (uint16_t) m3, (uint16_t) m4, throttle, rate_pv->x, rate_pv->y, rate_pv->z);
	usb_serial_write((const uint8_t*) temp, size);
#endif

	motor_set(m1, m2, m3, m4);
} 

void Chiindii::dispatch(FramedSerialMessage *request) {
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

void Chiindii::sendMessage(FramedSerialMessage *message) {
	protocol.write(&serial, message);
}

ISR(USART1_RX_vect){
	serial.isr();
}