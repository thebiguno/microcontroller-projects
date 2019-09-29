#include "Light.h"

#include <stdio.h>

#define PWM_MIN		0x3F
#define PWM_MAX		0xFF


#ifdef DEBUG
using namespace digitalcave;
extern SerialUSB serialUSB;
#endif

volatile uint8_t lightPinMaskYellow = 0;
volatile uint8_t lightPinMaskNeutral = 0;
volatile uint8_t lightPinMaskBlue = 0;

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

/*
 * We assign LIGHT1 through LIGHT3 to OCR1B, OCR1C, OCR3B respectively
 */
void light_on() {
	//WGM mode 7 (Fast PWM, 8 bit)
	TCCR1A = _BV(WGM10);
	TCCR3A = _BV(WGM30);

	//Enable timers with /8 prescaler
	TCCR1B = _BV(CS11) | _BV(WGM12);
	TCCR3B = _BV(CS31) | _BV(WGM32);

	TCCR1C = 0x00;
	TCCR3C = 0x00;

	//The PWM period is controlled by OCRnA (via the overflow interrupt); the phase of individual lights by OCRnB / OCRnC.
	TIMSK1 = _BV(OCIE1B) | _BV(OCIE1C) | _BV(TOIE1);
	TIMSK3 = _BV(OCIE3B) | _BV(TOIE3);

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void light_off(){
	LIGHT_PORT &= ~_BV(LIGHT_Y_PIN);		//OCR1B
	LIGHT_PORT &= ~_BV(LIGHT_N_PIN);		//OCR1C
	LIGHT_PORT &= ~_BV(LIGHT_B_PIN);		//OCR3B

	//Disable timers
	TCCR1B = 0x00;
	TCCR3B = 0x00;
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
	else if (brightnessYellowScaled < PWM_MIN){
		brightnessYellowScaled = PWM_MIN;
	}
	if (brightnessNeutralScaled > PWM_MAX){
		brightnessNeutralScaled = PWM_MAX;
	}
	else if (brightnessNeutralScaled < PWM_MIN){
		brightnessNeutralScaled = PWM_MIN;
	}
	if (brightnessBlueScaled > PWM_MAX){
		brightnessBlueScaled = PWM_MAX;
	}
	else if (brightnessBlueScaled < PWM_MIN){
		brightnessBlueScaled = PWM_MIN;
	}

	// OCR1B = brightnessYellowScaled;
	// OCR1C = brightnessNeutralScaled;
	// OCR3B = brightnessBlueScaled;
	//
	lightPinMaskYellow = _BV(LIGHT_Y_PIN);
	lightPinMaskNeutral = _BV(LIGHT_N_PIN);
	lightPinMaskBlue = _BV(LIGHT_B_PIN);

#ifdef DEBUG
	char temp[64];
	serialUSB.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "%6.5f, %6.5f, %d, %d, %d\n\r", brightness, whiteBalance, brightnessYellowScaled, brightnessNeutralScaled, brightnessBlueScaled));
#endif
}

//Turn on pins at overflow
ISR(TIMER1_OVF_vect){
	LIGHT_PORT |= lightPinMaskYellow;
	LIGHT_PORT |= lightPinMaskNeutral;
}
ISR(TIMER3_OVF_vect){
	LIGHT_PORT |= lightPinMaskBlue;
}

//Turn off pins on compare match
ISR(TIMER1_COMPB_vect){
	LIGHT_PORT &= ~_BV(LIGHT_Y_PIN);
	OCR1B = brightnessYellowScaled;
}
ISR(TIMER1_COMPC_vect){
	LIGHT_PORT &= ~_BV(LIGHT_N_PIN);
	OCR1C = brightnessNeutralScaled;
}
ISR(TIMER3_COMPB_vect){
	LIGHT_PORT &= ~_BV(LIGHT_B_PIN);
	OCR3B = brightnessBlueScaled;
}
