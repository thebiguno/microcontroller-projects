#include "motor.h"
#include "../Chiindii.h"

#define PWM_MAX		0x03FF

/*
 * We assign MOTOR1 through MOTOR4 to OCR1B, OCR1C, OCR3B, OCR3C respectively
 */
void motor_start() {
	//Set motor pins as output
	*(&MOTOR1_PORT - 0x01) |= _BV(MOTOR1_PIN);
	*(&MOTOR2_PORT - 0x01) |= _BV(MOTOR2_PIN);
	*(&MOTOR3_PORT - 0x01) |= _BV(MOTOR3_PIN);
	*(&MOTOR4_PORT - 0x01) |= _BV(MOTOR4_PIN);
	
	//WGM mode 11
	TCCR1A = _BV(WGM11) | _BV(WGM10);
	TCCR3A = _BV(WGM31) | _BV(WGM30);

	//Enable timers with no prescaler
	TCCR1B = _BV(CS10) | _BV(WGM13);
	TCCR3B = _BV(CS30) | _BV(WGM33);

	TCCR1C = 0x00;
	TCCR3C = 0x00;
	
	//This defines the max PWM value
	OCR1A = PWM_MAX;
	OCR3A = PWM_MAX;
	
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
	OCR1B = motorValue1 & PWM_MAX;
	OCR1C = motorValue2 & PWM_MAX;
	OCR3B = motorValue3 & PWM_MAX;
	OCR3C = motorValue4 & PWM_MAX;
}

EMPTY_INTERRUPT(TIMER1_COMPA_vect)
EMPTY_INTERRUPT(TIMER3_COMPA_vect)


//Turn on pins at overflow
ISR(TIMER1_OVF_vect){
	MOTOR1_PORT |= _BV(MOTOR1_PIN);
	MOTOR2_PORT |= _BV(MOTOR2_PIN);
}
ISR(TIMER3_OVF_vect){
	MOTOR3_PORT |= _BV(MOTOR3_PIN);
	MOTOR4_PORT |= _BV(MOTOR4_PIN);
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