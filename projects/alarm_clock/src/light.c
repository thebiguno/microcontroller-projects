#include "light.h"

#define PWM_MAX		0xFF

volatile uint8_t lightMask = 0;

void light_init(){
	//Set light pins as output
	*(&LIGHT_PORT - 0x01) |= _BV(LIGHT_Y_PIN);
	*(&LIGHT_PORT - 0x01) |= _BV(LIGHT_N_PIN);
	*(&LIGHT_PORT - 0x01) |= _BV(LIGHT_B_PIN);

	light_off();
}

//We assign LIGHT1 through LIGHT3 to OCR1A, OCR1B, OCR1C respectively
void light_on() {
	//WGM mode 7 (Fast PWM, 8 bit)
	TCCR1A = _BV(WGM10);

	//Enable timer with /64 prescaler
	TCCR1B = _BV(CS12) | _BV(WGM12);
	TCCR1C = 0x00;

	//The PWM period is controlled by OCRnA (via the overflow interrupt); the phase of individual lights by OCRnB / OCRnC.
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B) | _BV(OCIE1C) | _BV(TOIE1);

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void light_off(){
	LIGHT_PORT &= ~_BV(LIGHT_Y_PIN);		//OCR1A
	LIGHT_PORT &= ~_BV(LIGHT_N_PIN);		//OCR1B
	LIGHT_PORT &= ~_BV(LIGHT_B_PIN);		//OCR1C

	//Disable timers
	TCCR1B = 0x00;
}

uint8_t light_state(){
	if (TCCR1B == 0x00){
		return 0;
	}
	else {
		return 1;
	}
}

void light_toggle(){
	if (TCCR1B == 0x00){
		light_on();
	}
	else {
		light_off();
	}
}

void light_set(uint8_t brightnessi){
	//brightnessi is an unsigned int between 1 and 100.  Convert to a float for doing math.
	float brightness = brightnessi / 100.0;
	if (brightness < 0) brightness = 0;
	else if (brightness > 1) brightness = 1;

	//We use an exponential function to map the brightness to percieved brightness,
	// since human vision is logarithmic.  Brightness should vary from 0 to PWM_MAX.
	// The brighter the light, the more blue there is in it.
	float yellow = pow(brightness * 16, 2);
	float neutral = pow((brightness - 0.25) * 8.5, 3);
	float blue = pow((brightness - 0.5) * 12.7, 3);

	if (yellow < 1) yellow = 1;		//At the dimmest, we have yellow on for 1/256 value
	else if (yellow > PWM_MAX) yellow = PWM_MAX;

	if (neutral < 0) neutral = 0;
	else if (neutral > PWM_MAX) neutral = PWM_MAX;

	if (blue < 0) blue = 0;
	else if (blue > PWM_MAX) blue = PWM_MAX;

	OCR1A = (uint16_t) yellow;
	OCR1B = (uint16_t) neutral;
	OCR1C = (uint16_t) blue;

	lightMask = (OCR1A ? _BV(LIGHT_Y_PIN) : 0) | (OCR1B ? _BV(LIGHT_N_PIN) : 0) | (OCR1C ? _BV(LIGHT_B_PIN) : 0);
}

//Turn on pins at overflow
ISR(TIMER1_OVF_vect){
	LIGHT_PORT |= lightMask;
}

//We turn lights off at compare match.  To keep the ISRs as fast as possible, we do this in assembly, in light_asm.S
