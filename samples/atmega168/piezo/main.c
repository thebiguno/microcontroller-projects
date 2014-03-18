#include <avr/io.h>
#include <util/delay.h>

/**
 * Hardware:
 * Connect a piezo buzzer to pins D2 and D3.  We alternately drive
 * these pins high / low; by driving both pins, we get about 2x the
 * volume from the pizeo than if we only drove one side with the other
 * connected to ground.
 */

int main (void){
	//Do setup here
	DDRD |= _BV(PIND2) | _BV(PIND3);
	PORTD = _BV(PIND2);

	uint8_t delayDelta = 1;
	uint16_t delay = 100;

	//Main program loop
	while (1){
		_delay_us(delay + 1);
		PORTD = ~PORTD;
		_delay_us(delay);
		PORTD = ~PORTD;
		delay += ((delay / delayDelta) + 1);
		if (delay > 10000) {
			delay = 0;
			delayDelta--;
		}
		if (delayDelta == 0) delayDelta = 100;
	}
}
