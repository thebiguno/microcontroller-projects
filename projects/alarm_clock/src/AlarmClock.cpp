#include "AlarmClock.h"

#include <SerialUSB.h>

using namespace digitalcave;

SerialUSB serial;

int main(){
	timer_init();

	I2CAVR i2c;
	SPIStreamAVR spi;
	MAX7219 display(&spi, 4);
	DS3231 calendar(&i2c);
//	_delay_ms(5000);
//	serial.write("Starting");

	// DDRB |= _BV(PORTB4);

	Buffer buffer(32, 8);

	int8_t brightnessDirection = 1;
	int8_t whiteBalanceDirection = 1;

	// ds3231_time_t timeSet{19, 9, 1, 0, 16, 38, 0, DS3231_MODE_24};
	// calendar.setTime(timeSet);

	light_start();
	double brightness = 0;
	double whiteBalance = -1;

	//Main program loop
	while (1){
		ds3231_time_t time = calendar.getTime();
		_delay_ms(100);
		//PORTB ^= _BV(PORTB4);

		buffer.clear();
		char temp[32];
		snprintf(temp, sizeof(temp), "%02d:%02d:%02d", time.hour, time.minute, time.second);
		buffer.write_string(temp, font_3x5, 1, 1);
		display.write_buffer(buffer.get_data());

		light_set(1, whiteBalance);

		brightness += 0.01 * brightnessDirection;
		whiteBalance += 0.005 * whiteBalanceDirection;

		if (brightness < 0 || brightness > 1){
			brightnessDirection *= -1;
		}
		if (whiteBalance < -1 || whiteBalance > 1){
			whiteBalanceDirection *= -1;
		}

		// snprintf(temp, sizeof(temp), "%6.5f, %6.5f, %d, %d\n\r", brightness, whiteBalance, brightnessDirection, whiteBalanceDirection);
		// serial.write(temp);
	}
}
