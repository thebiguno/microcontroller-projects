#include "Light.h"

#include <stdio.h>
#include <Serial/SerialUSB.h>

#define PWM_MIN		0x01
#define PWM_MAX		0x3FF

using namespace digitalcave;

volatile uint8_t lightPinMaskYellow = 0;
volatile uint8_t lightPinMaskNeutral = 0;
volatile uint8_t lightPinMaskBlue = 0;

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
	//WGM mode 7 (Fast PWM, 10 bit)
	TCCR1A = _BV(WGM11) | _BV(WGM10);
	TCCR3A = _BV(WGM31) | _BV(WGM30);

	//Enable timers with /64 prescaler
	TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);
	TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);

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

void light_set(double brightness, double whiteBalance){
	if (brightness < 0) brightness = 0;
	else if (brightness > 1) brightness = 1;
	if (whiteBalance < -1) whiteBalance = -1;
	else if (whiteBalance > 1) whiteBalance = 1;

	//We use an eponential function to map the brightness to percieved brightness,
	// since human vision is logarithmic.  Brightness should vary from 0 to PWM_MAX.
	uint16_t brightnessNeutralScaled = pow(brightness * 32, 2);
	uint16_t whiteBalanceScaled = pow(whiteBalance * 32, 2);


	//The neutral LED is always mapped to brightness.
	//For the yellow LED, we subtract
	//whiteBalanceScaled values when whiteBalance is greater than 0.  For blue, we subtract
	//when values are less than 0.
	uint16_t brightnessYellowScaled = brightnessNeutralScaled;
	uint16_t brightnessBlueScaled = brightnessNeutralScaled;
	if (whiteBalance < 0){
		if (brightnessYellowScaled > whiteBalanceScaled){
			brightnessYellowScaled -= whiteBalanceScaled;
		}
		else {
			brightnessYellowScaled = 0;
		}
	}
	else if (whiteBalance > 0){
		if (brightnessBlueScaled > whiteBalanceScaled){
			brightnessBlueScaled -= whiteBalanceScaled;
		}
		else {
			brightnessBlueScaled = 0;
		}
	}

	if (brightnessYellowScaled > PWM_MAX) brightnessYellowScaled = PWM_MAX;
	if (brightnessNeutralScaled > PWM_MAX) brightnessNeutralScaled = PWM_MAX;
	if (brightnessBlueScaled > PWM_MAX) brightnessBlueScaled = PWM_MAX;

	OCR1B = brightnessYellowScaled;
	OCR1C = brightnessNeutralScaled;
	OCR3B = brightnessBlueScaled;

	//Anything less than PWM_MIN turns the lights off completely; this is so that we don't miss
	// low values when the compares fire before the overflow is completed
	if (brightnessYellowScaled < PWM_MIN) lightPinMaskYellow = 0x00;
	else lightPinMaskYellow = _BV(LIGHT_Y_PIN);
	if (brightnessNeutralScaled < PWM_MIN) lightPinMaskNeutral = 0x00;
	else lightPinMaskNeutral = _BV(LIGHT_N_PIN);
	if (brightnessBlueScaled < PWM_MIN) lightPinMaskBlue = 0x00;
	else lightPinMaskBlue = _BV(LIGHT_B_PIN);

	// char temp[64];
	// snprintf(temp, sizeof(temp), "%6.5f, %6.5f, %d, %d, %d\n\r", brightness, whiteBalance, brightnessYellowScaled, brightnessNeutralScaled, brightnessBlueScaled);
	// serial.write(temp);
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
}
ISR(TIMER1_COMPC_vect){
	LIGHT_PORT &= ~_BV(LIGHT_N_PIN);
}
ISR(TIMER3_COMPB_vect){
	LIGHT_PORT &= ~_BV(LIGHT_B_PIN);
}
