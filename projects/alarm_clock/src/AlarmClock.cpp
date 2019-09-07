#include "AlarmClock.h"

using namespace digitalcave;

SerialUSB serial;

int main(){
	// I2CAVR i2c;
	// DS3231 calendar(&i2c);

	// int8_t brightnessDirection = 1;
	// int8_t whiteBalanceDirection = 1;
	//
	// light_start();
	// double brightness = 0;
	// double whiteBalance = -1;

	State state;
	Display display;

	//Main program loop
	while (1){
		state.poll();

		display.update(state);

		delay_ms(100);	//TODO Change this back to 100 for better debouncing
		//
		// ds3231_time_t time = calendar.getTime();
		// _delay_ms(100);
		// //PORTB ^= _BV(PORTB4);
		//
		// buffer.clear();
		// char temp[32];
		// snprintf(temp, sizeof(temp), "%02d:%02d:%02d", time.hour, time.minute, time.second);
		// buffer.write_string(temp, font_3x5, 1, 1);
		// display.write_buffer(buffer.get_data());
		//
		// light_set(1, whiteBalance);
		//
		// brightness += 0.01 * brightnessDirection;
		// whiteBalance += 0.005 * whiteBalanceDirection;
		//
		// if (brightness < 0 || brightness > 1){
		// 	brightnessDirection *= -1;
		// }
		// if (whiteBalance < -1 || whiteBalance > 1){
		// 	whiteBalanceDirection *= -1;
		// }
		//
		// snprintf(temp, sizeof(temp), "%d\n\r", encoder.get_encoder_movement());
		// serial.write(temp);
	}
}
