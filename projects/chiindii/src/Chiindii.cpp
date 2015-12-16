#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <PID.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "lib/timer/timer.h"

#include "Chiindii.h"

#include "pwm/pwm.h"

using namespace digitalcave;

void drive_motors(double throttle, vector_t rate);

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	timer_init();
	pwm_init(&MOTOR1_PORT, MOTOR1_PIN, 
		&MOTOR2_PORT, MOTOR2_PIN,
		&MOTOR3_PORT, MOTOR3_PIN,
		&MOTOR4_PORT, MOTOR4_PIN);
	
	Mpu6050 mpu6050;
//	mpu6050.calibrate();

	// TODO supply proper kp, ki, kd
	PID rate_x(1, 0, 0, DIRECTION_NORMAL, 1, 0); // 1 = 1 ms
	PID rate_y(1, 0, 0, DIRECTION_NORMAL, 1, 0); // 1 = 1 ms
	PID rate_z(1, 0, 0, DIRECTION_NORMAL, 1, 0); // 1 = 1 ms
	
	DDRF |= _BV(5) | _BV(6) | _BV(7);
	DDRB |= _BV(0) | _BV(1);
	
	//Set motor pins as output
	*(&MOTOR1_PORT - 0x01) |= _BV(MOTOR1_PIN);
	*(&MOTOR2_PORT - 0x01) |= _BV(MOTOR2_PIN);
	*(&MOTOR3_PORT - 0x01) |= _BV(MOTOR3_PIN);
	*(&MOTOR4_PORT - 0x01) |= _BV(MOTOR4_PIN);
	
	double throttle = 0;
	vector_t rate_sp;
	vector_t rate;
	
	uint8_t motor = 0;

	//Main program loop
	while (1) {
/*
		vector_t accel = mpu6050.getAccel();
		vector_t gyro = mpu6050.getGyro();

		uint32_t time = timer_millis();
		rate_x.compute(rate_sp.x, gyro.x, &rate.x, time);
		rate_y.compute(rate_sp.y, gyro.y, &rate.y, time);
		rate_z.compute(rate_sp.z, gyro.z, &rate.z, time);
		drive_motors(throttle, rate);
*/
		
		PORTB ^= _BV(0) | _BV(1);
		PORTF ^= _BV(5) | _BV(6) | _BV(7);

		pwm_set_values(motor, motor + 16, motor + 32, motor + 48);
		motor++;
		
		_delay_ms(100);
	}
}

void drive_motors(double throttle, vector_t rate) {
	double m1 = throttle - rate.x - rate.y - rate.z;
	double m2 = throttle + rate.x - rate.y + rate.z;
	double m3 = throttle + rate.x + rate.y - rate.z;
	double m4 = throttle - rate.x + rate.y + rate.z;

	pwm_set_values(m1, m2, m3, m4);
} 
