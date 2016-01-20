#include "battery.h"
#include "../lib/usb/serial.h"

void battery_init(){
	//Enable ADC with div32 prescaler, single shot conversions
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0);
	
	//Set ADMUX to ADC11, with left adjust result and 2.56v internal reference
	ADCSRB = _BV(MUX5);
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | _BV(MUX1) | _BV(MUX0);
	
	//Disable digital input on ADC11
	DIDR2 = _BV(ADC11D);
}

uint8_t battery_read(){
	uint8_t result = ADCH;
	
	//Start conversion for next time
	ADCSRA |= _BV(ADSC);

	return result;
}