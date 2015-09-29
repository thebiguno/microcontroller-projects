#include "Analog.h"

using namespace digitalcave;

Analog::Analog(){
	//Set AREF to AVCC, with left adjusted result (we only care about 8 MSB)
	ADMUX = _BV(REFS0) | _BV(ADLAR);
	//ADC Enable, prescaler to 0x4 (F_CPU / 16)
	ADCSRA = _BV(ADEN) | _BV(ADPS2);
	//Disable digital pins
	DIDR2 = _BV(ADC13D) | _BV(ADC12D);
	//Free running mode (already default)
	ADCSRB = 0x00;
}

uint8_t Analog::read(uint8_t pin){
	ADMUX &= 0xE0;
	ADCSRB &= ~_BV(MUX5);

	if (pin == 12){
		ADMUX |= _BV(MUX2);
		ADCSRB |= _BV(MUX5);
	}
	else if (pin == 13){
		ADMUX |= _BV(MUX2) | _BV(MUX0);
		ADCSRB |= _BV(MUX5);
	}
	else {
		return 0;
	}
	
	//Start conversion
	ADCSRA |= _BV(ADSC);
	
	//Wait until conversion is complete
	while (ADCSRA & _BV(ADSC));

	//Return conversion result
	return ADCH;
}
