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

	DDRB |= _BV(PORTB4);

	Buffer buffer(32, 8);

	int16_t i = 0;
	int8_t direction = -1;

	// ds3231_time_t timeSet{19, 9, 1, 0, 16, 38, 0, DS3231_MODE_24};
	// calendar.setTime(timeSet);

	//Main program loop
	while (1){
		ds3231_time_t time = calendar.getTime();

		_delay_ms(100);
		PORTB ^= _BV(PORTB4);

		buffer.clear();
		char temp[32];
		snprintf(temp, sizeof(temp), "%02d:%02d:%02d", time.hour, time.minute, time.second);
		buffer.write_string(temp, font_3x5, 1, 1);
		display.write_buffer(buffer.get_data());

		i += direction;

		if (i < -68 || i > 30){
			direction *= -1;
		}
	}
}
