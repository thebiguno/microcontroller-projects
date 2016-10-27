#include <stdio.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <SerialUSB.h>

#include <MPU6050/MPU6050.h>
#include <I2C/I2CAVR.h>

using namespace digitalcave;

int main (){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);

	DDRB |= _BV(4) | _BV(5) | _BV(6);
	PORTB |= _BV(4) | _BV(5) | _BV(6);

	SerialUSB serial;

	for (uint8_t i = 0; i < 10; i++){
		PORTB ^= _BV(4);
		_delay_ms(100);
	}

	I2CAVR i2c;
	MPU6050 mpu6050(&i2c);
	mpu6050.calibrate();

	for (uint8_t i = 0; i < 10; i++){
		PORTB ^= _BV(6);
		_delay_ms(100);
	}

	char temp[128];
	uint8_t raw[14];

	vector_t accel, gyro;
	double temperature;

	int16_t* cal = mpu6050.getCalibration();
	uint8_t size = snprintf(temp, sizeof(temp), "Cal: %d\t%d\t%d\t%d\t%d\t%d\t\n", cal[0], cal[1], cal[2], cal[3], cal[4], cal[5]);
	serial.write((uint8_t*) temp, size);

	while (1){
		mpu6050.getRaw(raw);
		uint8_t size = snprintf(temp, sizeof(temp), "Raw: %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7], raw[8], raw[9], raw[10], raw[11], raw[12], raw[13]);
		serial.write((uint8_t*) temp, size);

		mpu6050.getValuesFromRaw(&accel, &gyro, &temperature, raw);

		size = snprintf(temp, sizeof(temp), "Accel: X: %.02f  Y: %.02f  Z: %.02f\n", accel.x, accel.y, accel.z);
		serial.write((uint8_t*) temp, size);
		size = snprintf(temp, sizeof(temp), "Gyro: X: %.02f  Y: %.02f  Z: %.02f\n", gyro.x, gyro.y, gyro.z);
		serial.write((uint8_t*) temp, size);
		size = snprintf(temp, sizeof(temp), "Temperature: %.1f\n", temperature);
		serial.write((uint8_t*) temp, size);

		//serial.write("Foo\n");

		PORTB ^= _BV(5);
		_delay_ms(1000);
	}

	return 0;
}
