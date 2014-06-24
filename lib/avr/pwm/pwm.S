#include <avr/io.h>

#ifdef PWM_8_BIT
	.global TIM0_COMPB_vect
TIM0_COMPB_vect:
#else
	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:
#endif


	;Save calling state
	PUSH		r30
	PUSH		r31
	PUSH		r20
#ifndef PWM_8_BIT
	PUSH		r21
#endif

	;Load the pointer address into register Z
	LDI			r30,					lo8(_pwm_events_low_ptr)
	LDI			r31,					hi8(_pwm_events_low_ptr)

	;Load portA low mask and increment pointer to portB mask
	LD			r20,					Z+
#ifndef PWM_PORTA_UNUSED
	; PORTA &= porta mask
	AND			r20,					_SFR_IO_ADDR(PORTA)
	OUT			_SFR_IO_ADDR(PORTA),	r20
#endif


	;Load portB low mask and increment pointer to portC mask
	LD			r20,					Z+
#ifndef PWM_PORTB_UNUSED
	; PORTB &= portb mask
	AND			r20,					_SFR_IO_ADDR(PORTB)
	OUT			_SFR_IO_ADDR(PORTB),	r20
#endif


	;Load portC low mask and increment pointer to portD mask
	LD			r20,					Z+
#ifndef PWM_PORTC_UNUSED
	; PORTC &= portc mask
	AND			r20,					_SFR_IO_ADDR(PORTC)
	OUT			_SFR_IO_ADDR(PORTC),	r20
#endif


	;Load portD low mask and increment pointer to compare value
	LD			r20,					Z+
#ifndef PWM_PORTD_UNUSED
	; PORTD &= portd mask
	AND			r20,					_SFR_IO_ADDR(PORTD)
	OUT			_SFR_IO_ADDR(PORTD),	r20
#endif

	;Load the new compare value into OCRnB
#ifdef PWM_8_BIT
	LD			r20,					Z+
	STS			OCR0B,					r20

#else
	LD			r20,					Z+
	LD			r21,					Z+
	STS			OCR1BH,					r20
	STS			OCR1BL,					r21
#endif

	;Save register Z back to pointer address
	STS			_pwm_events_low_ptr+1,	r31
	STS			_pwm_events_low_ptr,	r30

			//Restore calling state
#ifndef PWM_8_BIT
	POP			r21
#endif
	POP			r20
	POP			r31
	POP			r30
	
	RETI