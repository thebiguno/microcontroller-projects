#include "MPUTest.h"

#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>
#include <string.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include <PID.h>
#include <SerialUSB.h>

#include "lib/Mpu6050/Mpu6050.h"
#include "timer/timer.h"

#include "motor/motor.h"

#define EEPROM_OFFSET	0x300

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialUSB serial;

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	timer_init();
	
	MPUTest mpuTest;
	mpuTest.run();
}

void MPUTest::sendDebug(const char* message){
	sendDebug((char*) message);
}
void MPUTest::sendDebug(char* message){
	serial.write((uint8_t*) message, strnlen(message, 128));
}

MPUTest::MPUTest() :
	//Attitude filter period determines PID period
	c_x(0.15, 0),
	c_y(0.15, 0)
{
}

void MPUTest::run() {
	uint32_t time = 0;
	
	int16_t calibration[3];
	eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 80), 6);
	mpu6050.setAccelCalib(calibration);
	eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 86), 6);
	mpu6050.setGyroCalib(calibration);
	
	vector_t gyro;
	vector_t accel;
	vector_t angle_mv;
	vector_t maxGyro = {0, 0, 0};
	vector_t maxAccel = {0, 0, 1};
	vector_t maxAngle = {0, 0, 0};
	vector_t minGyro = {0, 0, 0};
	vector_t minAccel = {0, 0, 1};
	vector_t minAngle = {0, 0, 0};
	vector_t totalGyro = {0, 0, 0};
	vector_t totalAccel = {0, 0, 0};
	vector_t totalAngle = {0, 0, 0};
	
	motor_start();
	
	_delay_ms(1000);
	char temp[128];

#define COUNT	200
	
	motor_set(300, 300, 300, 300);
	for (uint16_t i = 0; i < COUNT; i++){
		time = timer_millis();
		
		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();

		// compute the absolute angle relative to the horizontal.  We use the standard convention of
		// rotation about the X axis is roll, and rotation about the Y axis is pitch.
		// See http://stackoverflow.com/questions/3755059/3d-accelerometer-calculate-the-orientation, answer by 'matteo' for formula.
		angle_mv.x = atan2(accel.y, accel.z);
		angle_mv.y = atan2(-accel.x, sqrt(accel.y * accel.y + accel.z * accel.z));

		// complementary tuning
		// filter gyro rate and measured angle increase the accuracy of the angle
		angle_mv.x = c_x.compute(gyro.x, angle_mv.x, time);
		angle_mv.y= c_y.compute(gyro.y, angle_mv.y, time);

		if (gyro.x < minGyro.x) minGyro.x = gyro.x;
		if (gyro.y < minGyro.y) minGyro.y = gyro.y;
		if (gyro.z < minGyro.z) minGyro.z = gyro.z;
		if (gyro.x > maxGyro.x) maxGyro.x = gyro.x;
		if (gyro.y > maxGyro.y) maxGyro.y = gyro.y;
		if (gyro.z > maxGyro.z) maxGyro.z = gyro.z;
		if (accel.x < minAccel.x) minAccel.x = accel.x;
		if (accel.y < minAccel.y) minAccel.y = accel.y;
		if (accel.z < minAccel.z) minAccel.z = accel.z;
		if (accel.x > maxAccel.x) maxAccel.x = accel.x;
		if (accel.y > maxAccel.y) maxAccel.y = accel.y;
		if (accel.z > maxAccel.z) maxAccel.z = accel.z;
		if (angle_mv.x < minAngle.x) minAngle.x = angle_mv.x;
		if (angle_mv.y < minAngle.y) minAngle.y = angle_mv.y;
		if (angle_mv.x > maxAngle.x) maxAngle.x = angle_mv.x;
		if (angle_mv.y > maxAngle.y) maxAngle.y = angle_mv.y;
		
		totalAccel.x += accel.x;
		totalAccel.y += accel.y;
		totalAccel.z += accel.z;
		totalGyro.x += gyro.x;
		totalGyro.y += gyro.y;
		totalGyro.z += gyro.z;
		totalAngle.x += angle_mv.x;
		totalAngle.y += angle_mv.y;
		
		_delay_ms(10);
	}
	motor_set(0, 0, 0, 0);
	
	snprintf(temp, sizeof(temp), "Max Accel: %3.2f, %3.2f, %3.2f; Max Gyro: %3.2f, %3.2f, %3.2f; Max Angle: %3.0f, %3.0f\n", maxAccel.x, maxAccel.y, maxAccel.z, maxGyro.x, maxGyro.y, maxGyro.z, RADIANS_TO_DEGREES(maxAngle.x), RADIANS_TO_DEGREES(maxAngle.y));
	sendDebug(temp);
	snprintf(temp, sizeof(temp), "Min Accel: %3.2f, %3.2f, %3.2f; Min Gyro: %3.2f, %3.2f, %3.2f; Min Angle: %3.0f, %3.0f\n", minAccel.x, minAccel.y, minAccel.z, minGyro.x, minGyro.y, minGyro.z, RADIANS_TO_DEGREES(minAngle.x), RADIANS_TO_DEGREES(minAngle.y));
	sendDebug(temp);
	snprintf(temp, sizeof(temp), "Avg Accel: %3.2f, %3.2f, %3.2f; Avg Gyro: %3.2f, %3.2f, %3.2f; Avg Angle: %3.0f, %3.0f\n", totalAccel.x / COUNT, totalAccel.y / COUNT, totalAccel.z / COUNT, totalGyro.x / COUNT, totalGyro.y / COUNT, totalGyro.z / COUNT, RADIANS_TO_DEGREES(totalAngle.x / COUNT), RADIANS_TO_DEGREES(totalAngle.y / COUNT));
	sendDebug(temp);


	//Main program loop
	while (1) {
	}
}