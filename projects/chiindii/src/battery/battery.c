#include "battery.h"
#include "../lib/usb/serial.h"

void battery_init(){
	//Enable ADC with div32 prescaler, free running
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0) | _BV(ADATE);
	
	//Set ADMUX to ADC11, with left adjust result and 2.56v internal reference
	ADCSRB = _BV(MUX5);
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | _BV(MUX1) | _BV(MUX0);
	
	//Disable digital input on ADC11
	DIDR2 = _BV(ADC11D);

	//Start free running conversions
	ADCSRA |= _BV(ADSC);
}

uint8_t battery_read(){
	return ADCH;
}