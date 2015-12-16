#include <stdio.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/Mpu6050/Mpu6050.h"

#include "lib/twi/twi.h"

using namespace digitalcave;

int main (){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	DDRF |= _BV(5) | _BV(6) | _BV(7);
	PORTF |= _BV(5) | _BV(6) | _BV(7);
	
	// initialize the USB, and then wait for the host
	// to set configuration.  If the Teensy is powered
	// without a PC connected to the USB port, this 
	// will wait forever.
	//usb_init();
	//while (!usb_configured()) /* wait */ ;
	PORTF ^= _BV(5) | _BV(6) | _BV(7);
	_delay_ms(1000);
	PORTF ^= _BV(5) | _BV(6) | _BV(7);
	
	Mpu6050 mpu6050;
	mpu6050.calibrate();

	PORTF ^= _BV(5) | _BV(6) | _BV(7);
	_delay_ms(1000);
	PORTF ^= _BV(5) | _BV(6) | _BV(7);

	//char temp[64];
	
	while (1){
		vector_t accel = mpu6050.getAccel();
		vector_t gyro = mpu6050.getGyro();
		double temperature = mpu6050.getTemperature();
		
		//snprintf(temp, sizeof(temp), "\n\nAX: %.02f\nY: %.02f\nZ: %.02f\nGX: %.02f\nY: %.02f\nZ: %.02f\nT: %.0f\n                                                       ", accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z, temperature);
		
		//usb_serial_flush_input();
		//usb_serial_write((uint8_t*) temp, sizeof(temp));
		
		PORTF ^= _BV(5) | _BV(6) | _BV(7);
		_delay_ms(1000);
	}
}
