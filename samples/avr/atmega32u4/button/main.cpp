#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include <ButtonAVR.h>

using namespace digitalcave;

int main (){
	ButtonAVR button(&PORTE, PORTE2, 50, 25, 1000, 500);

	DDRB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);

	uint32_t systemTime = 0;

	while (1){
		button.sample(systemTime);
		if (button.releaseEvent()) PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6);
		if (button.pressEvent()) PORTB ^= _BV(PORTB4);
		if (button.longPressEvent()) PORTB ^= _BV(PORTB6);
		if (button.repeatPressEvent()) PORTB ^= _BV(PORTB5);

		_delay_ms(10);
		systemTime+=10;
	}
}
