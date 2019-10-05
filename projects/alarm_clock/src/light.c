#include "light.h"

#define PWM_MAX		0xFF

volatile uint8_t lightMask = 0;

volatile uint16_t brightnessNeutralScaled = 0;
volatile uint16_t brightnessYellowScaled = 0;
volatile uint16_t brightnessBlueScaled = 0;


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

void light_set(double brightness, double whiteBalance){
	//brightness is a float between 0 and 1.
	//whiteBalance is a float between -1 and 1.
	if (brightness < 0) brightness = 0;
	else if (brightness > 1) brightness = 1;
	if (whiteBalance < -1) whiteBalance = -1;
	else if (whiteBalance > 1) whiteBalance = 1;

	//We use an exponential function to map the brightness to percieved brightness,
	// since human vision is logarithmic.  Brightness should vary from 0 to PWM_MAX.
	brightnessNeutralScaled = pow(brightness * 16, 2);
	uint16_t whiteBalanceScaled = pow(whiteBalance * 16, 2);


	//The neutral LED is always mapped to brightness.
	//For the yellow LED, we subtract
	//whiteBalanceScaled values when whiteBalance is greater than 0.  For blue, we subtract
	//when values are less than 0.
	brightnessYellowScaled = brightnessNeutralScaled;
	brightnessBlueScaled = brightnessNeutralScaled;
	if (whiteBalance < 0){
		if (brightnessBlueScaled > whiteBalanceScaled){
			brightnessBlueScaled -= whiteBalanceScaled;
		}
		else {
			brightnessBlueScaled = 0;
		}
	}
	else if (whiteBalance > 0){
		if (brightnessYellowScaled > whiteBalanceScaled){
			brightnessYellowScaled -= whiteBalanceScaled;
		}
		else {
			brightnessYellowScaled = 0;
		}
	}

	if (brightnessYellowScaled > PWM_MAX){
		brightnessYellowScaled = PWM_MAX;
	}
	if (brightnessNeutralScaled > PWM_MAX){
		brightnessNeutralScaled = PWM_MAX;
	}
	if (brightnessBlueScaled > PWM_MAX){
		brightnessBlueScaled = PWM_MAX;
	}

	OCR1A = brightnessYellowScaled;
	OCR1B = brightnessNeutralScaled;
	OCR1C = brightnessBlueScaled;

	lightMask = (brightnessYellowScaled ? _BV(LIGHT_Y_PIN) : 0) | (brightnessNeutralScaled ? _BV(LIGHT_N_PIN) : 0) | (brightnessBlueScaled ? _BV(LIGHT_B_PIN) : 0);
}

//Turn on pins at overflow
ISR(TIMER1_OVF_vect){
	LIGHT_PORT |= lightMask;
}

//We turn lights off at compare match.  To keep the ISRs as fast as possible, we do this in assembly, in light_asm.S
