/*
 * 
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

//uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void get_mcusr(void) __attribute__((naked))  __attribute__((section(".init3")));
void get_mcusr(void) {
//	mcusr_mirror = MCUSR;
	MCUSR = 0;
	wdt_disable();
}

int main (void){
	//Do setup here
	DDRB = _BV(PORTB5);

	for (uint8_t i = 0; i < 10; i++){
		PORTB ^= _BV(PORTB5);
		_delay_ms(100);
	}
	
	wdt_enable(WDTO_120MS);
	
	//Main program loop
	while (1){
		PORTB ^= _BV(PORTB5);
		_delay_ms(500);
	}
}

