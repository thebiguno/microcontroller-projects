#include "battery.h"

void battery_init(){
	// //Enable ADC with div32 prescaler, single shot conversions
	// ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS0);
	//
	// //Set ADMUX to ADC11, with left adjust result and 2.56v internal reference
	// ADCSRB = _BV(MUX5);
	// ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR) | _BV(MUX1) | _BV(MUX0);
	//
	// //Disable digital input on ADC11
	// DIDR2 = _BV(ADC11D);
}

uint8_t battery_read(){
	// static uint16_t result = 0;		//The running average
	// uint8_t reading = ADCH;
	//
	// //Start conversion for next time
	// ADCSRA |= _BV(ADSC);
	//
	// //Keep an average of 256 (2^8) positions.  This is a LPF to ensure battery jitter doesn't
	// // cause false low readings
	// result = result + reading - (result >> 8);
	//
	// return result >> 8;
	return 0;	//TODO
}

uint8_t battery_pct() {
	// return (uint8_t) ((100.0 * ADCH) / 66.0) - 247.0;
	return 0;	//TODO
}
