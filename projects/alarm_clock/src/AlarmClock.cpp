#include "AlarmClock.h"

//#include <SerialUSB.h>

using namespace digitalcave;

//SerialUSB serial;

int main(){
	// char buffer[128];

	timer_init();

	SPIStreamAVR spi;
	MAX7219 display(&spi, 4);
//	_delay_ms(5000);
//	serial.write("Starting");

	DDRB |= _BV(PORTB4);

//	Si4703 si4703(&PORTB, PORTB6, &PORTB, PORTB5, &PORTD, PORTD1);
//	si4703.set_volume(5);

	Buffer buffer(32, 8);

	int16_t i = 0;
	int8_t direction = -1;

	//Main program loop
	while (1){
		_delay_ms(100);
		PORTB ^= _BV(PORTB4);

		buffer.clear();
		buffer.write_string("0123456789:AP", font_clockface, i, 0);
		display.write_buffer(buffer.get_data());

		i += direction;

		if (i < -68 || i > 30){
			direction *= -1;
		}
	}
}
