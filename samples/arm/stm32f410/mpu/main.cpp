#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>
#include <I2CHAL.h>
#include <HMC5883L.h>
#include <MS5611.h>
#include <MPU6050.h>
#include <dctypes.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart6;
extern I2C_HandleTypeDef hi2c2;

extern "C" {
	void dc_main();
}

void dc_main(){
	//We flash the light briefly, then go into the infinite loop.
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);

	SerialHAL serial(&huart6, 64);
	I2CHAL i2cHal(&hi2c2);
	MPU6050 mpu6050(&i2cHal);
	HMC5883L hmc5883l(&i2cHal);
	MS5611 ms5611(&i2cHal);
	mpu6050.calibrate();

	char temp[128];
	uint8_t raw[14];

	vector_t accel, gyro;
	double temperature;

	uint32_t lastSend = timer_millis();

	while (1){
		if ((timer_millis() - lastSend) > 1000){
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);

			uint8_t size;

			serial.write("MPU6050:\n");
			mpu6050.getRaw(raw);
			mpu6050.getValuesFromRaw(&accel, &gyro, &temperature, raw);
			size = snprintf(temp, sizeof(temp), "Raw: %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7], raw[8], raw[9], raw[10], raw[11], raw[12], raw[13]);
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Accel: X: %.02f  Y: %.02f  Z: %.02f\n", accel.x, accel.y, accel.z);
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Gyro: X: %.02f  Y: %.02f  Z: %.02f\n", gyro.x, gyro.y, gyro.z);
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Temperature: %.1f\n", temperature);
			serial.write((uint8_t*) temp, size);

			serial.write("HMC5883L:\n");
			hmc5883l.getRaw(raw);
			vector_t mag = hmc5883l.getValuesFromRaw(raw);
			size = snprintf(temp, sizeof(temp), "Raw: %02x%02x %02x%02x %02x%02x\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Mag: X: %.02f  Y: %.02f  Z: %.02f\n", mag.x, mag.y, mag.z);
			serial.write((uint8_t*) temp, size);


			serial.write("MS5611:\n");
			ms5611.getRaw(raw);
			size = snprintf(temp, sizeof(temp), "Raw: %02x%02x%02x %02x%02x%02x\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Pressure: %ld\n", ms5611.getPressureFromRaw(raw));
			serial.write((uint8_t*) temp, size);
			size = snprintf(temp, sizeof(temp), "Temperature: %ld\n", ms5611.getTemperatureFromRaw(raw));
			serial.write((uint8_t*) temp, size);

			serial.write("\n");

			// mpu6050.getValues(&accel, &gyro, &temperature);
			//
			// uint8_t size = 0;
			// char temp[64];
			// size = snprintf(temp, sizeof(temp), "Accel: X: %.02f  Y: %.02f  Z: %.02f\n", accel.x, accel.y, accel.z);
			// serial.write((uint8_t*) temp, size);
			// size = snprintf(temp, sizeof(temp), "Gyro:  X: %.02f  Y: %.02f  Z: %.02f\n", gyro.x, gyro.y, gyro.z);
			// serial.write((uint8_t*) temp, size);
			// size = snprintf(temp, sizeof(temp), "Temperature: %.1f\n", temperature);
			// serial.write((uint8_t*) temp, size);
			lastSend = timer_millis();
			//
			// uint8_t b = 0;
			// while (serial.read(&b)){
			// 	serial.write(b);
			// }
		}
	}
}
