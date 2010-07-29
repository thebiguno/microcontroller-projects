/*
 * LYPR530 implementation 
 */
#include "../gyro.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define M 0.006141921121388
#define B M_PI * -1

// TODO this may need to be a non-linear scale dependending on the measured output of the gyro
// If AREF = 3.3V, then A/D is 931 at 3V and 465 = 1.5V
// y = mx + b 
// m = (y2 - y1) / (x2 - x1) = (π - -π) / (1023 - 0) = 0.006141921121388
// b = y1 - m * x1 = -π - 0.006141921121388 * 0 = -π
#define SCALE_TO_RAD(x) (M * (x) + B)

static uint16_t results[3];
static uint8_t adc_mux = 0;

void gyro_init(){
	ADMUX = 0x00;

	//TODO configure ADPS2::0 (prescaler) to give speed / accuracy tradeoff
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | 4;  	//ADC Enable, ADC Interrupt Enable

	ADCSRB = 0x00;	//Free running mode

	//Disable digital input on pins 0..2
	DIDR0 |= _BV(0) | _BV(1) | _BV(2);

	//Start ADC conversions
	ADCSRA |= _BV(ADSC);

	//Send HP reset for a few ms
	DDRC |= _BV(PIN3);
	PORTC |= _BV(PIN3);
	_delay_ms(10);
	PORTC &= ~_BV(PIN3);
	DDRC &= ~_BV(PIN3);

	sei();
}

vector_t gyro_get() {
    vector_t result;
    result.x = SCALE_TO_RAD(results[0]);
    result.y = SCALE_TO_RAD(results[1]);
    result.z = SCALE_TO_RAD(results[2]);
    return result;
}

char temp[32];
ISR(ADC_vect){
	//Read last ADC value
	results[adc_mux] = ADC;

	//Increment to next pin and start again
	adc_mux = (adc_mux + 1) % 3;
	ADMUX = ((ADMUX) & 0xF0) | adc_mux;

	//Start ADC again
	ADCSRA |= _BV(ADSC);
}

