#include "battery.h"

void battery_init(){
	//Enable ADC with div32 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0);
	
	//Set ADMUX to ADC11, with left adjust result and 2.56v internal reference
	ADCSRB = _BV(MUX5);
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | _BV(MUX1) | _BV(MUX0);
}

uint8_t battery_read(){
	return ADCH;
}