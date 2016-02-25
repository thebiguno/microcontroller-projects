#include "motor.h"
#include "../Chiindii.h"

#define PWM_MIN		0x2F
#define PWM_MAX		0x1FF

volatile uint8_t motorPinMask1 = 0;
volatile uint8_t motorPinMask2 = 0;
volatile uint8_t motorPinMask3 = 0;
volatile uint8_t motorPinMask4 = 0;

/*
 * We assign MOTOR1 through MOTOR4 to OCR1B, OCR1C, OCR3B, OCR3C respectively
 */
void motor_start() {
	//Set motor pins as output
	*(&MOTOR1_PORT - 0x01) |= _BV(MOTOR1_PIN);
	*(&MOTOR2_PORT - 0x01) |= _BV(MOTOR2_PIN);
	*(&MOTOR3_PORT - 0x01) |= _BV(MOTOR3_PIN);
	*(&MOTOR4_PORT - 0x01) |= _BV(MOTOR4_PIN);
	
	//WGM mode 7 (Fast PWM, 9 bit)
	TCCR1A = _BV(WGM11);
	TCCR3A = _BV(WGM31);

	//Enable timers with no prescaler
	TCCR1B = _BV(CS10) | _BV(WGM12);
	TCCR3B = _BV(CS30) | _BV(WGM32);

	TCCR1C = 0x00;
	TCCR3C = 0x00;
	
	//The PWM period is controlled by OCRnA (via the overflow interrupt); the phase of individual motors by OCRnB / OCRnC.
	TIMSK1 = _BV(OCIE1B) | _BV(OCIE1C) | _BV(TOIE1);
	TIMSK3 = _BV(OCIE3B) | _BV(OCIE3C) | _BV(TOIE3);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void motor_stop(){
	MOTOR1_PORT &= ~_BV(MOTOR1_PIN);
	MOTOR2_PORT &= ~_BV(MOTOR2_PIN);
	MOTOR3_PORT &= ~_BV(MOTOR3_PIN);
	MOTOR4_PORT &= ~_BV(MOTOR4_PIN);
	
	//Disable timers
	TCCR1B = 0x00;
	TCCR3B = 0x00;
}

void motor_set(uint16_t motorValue1, uint16_t motorValue2, uint16_t motorValue3, uint16_t motorValue4){
	if (motorValue1 > PWM_MAX) motorValue1 = PWM_MAX;
	if (motorValue2 > PWM_MAX) motorValue2 = PWM_MAX;
	if (motorValue3 > PWM_MAX) motorValue3 = PWM_MAX;
	if (motorValue4 > PWM_MAX) motorValue4 = PWM_MAX;
	
	OCR1B = motorValue1;
	OCR1C = motorValue2;
	OCR3B = motorValue3;
	OCR3C = motorValue4;
	
	//Anything less than PWM_MIN turns the motors off completely; this is so that we don't miss
	// low values when the compares fire before the overflow is completed
	if (motorValue1 < PWM_MIN) motorPinMask1 = 0x00;
	else motorPinMask1 = _BV(MOTOR1_PIN);
	if (motorValue2 < PWM_MIN) motorPinMask2 = 0x00;
	else motorPinMask2 = _BV(MOTOR2_PIN);
	if (motorValue3 < PWM_MIN) motorPinMask3 = 0x00;
	else motorPinMask3 = _BV(MOTOR3_PIN);
	if (motorValue4 < PWM_MIN) motorPinMask4 = 0x00;
	else motorPinMask4 = _BV(MOTOR4_PIN);
}

//Turn on pins at overflow
ISR(TIMER1_OVF_vect){
	MOTOR1_PORT |= motorPinMask1;
	MOTOR2_PORT |= motorPinMask2;
}
ISR(TIMER3_OVF_vect){
	MOTOR3_PORT |= motorPinMask3;
	MOTOR4_PORT |= motorPinMask4;
}

//Turn off pins on compare match
ISR(TIMER1_COMPB_vect){
	MOTOR1_PORT &= ~_BV(MOTOR1_PIN);
}
ISR(TIMER1_COMPC_vect){
	MOTOR2_PORT &= ~_BV(MOTOR2_PIN);
}
ISR(TIMER3_COMPB_vect){
	MOTOR3_PORT &= ~_BV(MOTOR3_PIN);
}
ISR(TIMER3_COMPC_vect){
	MOTOR4_PORT &= ~_BV(MOTOR4_PIN);
}