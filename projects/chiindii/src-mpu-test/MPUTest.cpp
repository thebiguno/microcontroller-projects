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
	
	MPUTest mpuTest;
	mpuTest.run();
}

void MPUTest::sendDebug(const char* message){
	sendDebug((char*) message);
}
void MPUTest::sendDebug(char* message){
	serial.write((uint8_t*) message, strnlen(message, 128));
}

MPUTest::MPUTest() {
}

void MPUTest::run() {
	
	int16_t calibration[3];
	eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 80), 6);
	mpu6050.setAccelCalib(calibration);
	eeprom_read_block(calibration, (void*) (EEPROM_OFFSET + 86), 6);
	mpu6050.setGyroCalib(calibration);
	
	vector_t gyro;
	vector_t accel;
	vector_t maxGyro = {0, 0, 0};
	vector_t maxAccel = {0, 0, 1};
	vector_t minGyro = {0, 0, 0};
	vector_t minAccel = {0, 0, 1};
	vector_t totalGyro = {0, 0, 0};
	vector_t totalAccel = {0, 0, 0};
	
	motor_start();
	
	_delay_ms(1000);
	char temp[128];

#define COUNT	200
	
	motor_set(300, 300, 300, 300);
	for (uint16_t i = 0; i < COUNT; i++){
		accel = mpu6050.getAccel();
		gyro = mpu6050.getGyro();
		
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
		
		totalAccel.x += accel.x;
		totalAccel.y += accel.y;
		totalAccel.z += accel.z;
		totalGyro.x += gyro.x;
		totalGyro.y += gyro.y;
		totalGyro.z += gyro.z;
		
		snprintf(temp, sizeof(temp), "Raw Accel: %3.2f, %3.2f, %3.2f; Raw Gyro: %3.2f, %3.2f, %3.2f\n", accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z);
		sendDebug(temp);

		_delay_ms(10);
	}
	motor_set(0, 0, 0, 0);
	
	snprintf(temp, sizeof(temp), "Max Accel: %3.2f, %3.2f, %3.2f; Max Gyro: %3.2f, %3.2f, %3.2f\n", maxAccel.x, maxAccel.y, maxAccel.z, maxGyro.x, maxGyro.y, maxGyro.z);
	sendDebug(temp);
	snprintf(temp, sizeof(temp), "Min Accel: %3.2f, %3.2f, %3.2f; Min Gyro: %3.2f, %3.2f, %3.2f\n", minAccel.x, minAccel.y, minAccel.z, minGyro.x, minGyro.y, minGyro.z);
	sendDebug(temp);
	snprintf(temp, sizeof(temp), "Avg Accel: %3.2f, %3.2f, %3.2f; Avg Gyro: %3.2f, %3.2f, %3.2f\n", totalAccel.x / COUNT, totalAccel.y / COUNT, totalAccel.z / COUNT, totalGyro.x / COUNT, totalGyro.y / COUNT, totalGyro.z / COUNT);
	sendDebug(temp);


	//Main program loop
	while (1) {
	}
}