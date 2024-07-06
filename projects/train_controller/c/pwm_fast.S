#include <avr/io.h>

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:

	;Save calling state
	PUSH		r0
	IN			r0,						_SFR_IO_ADDR(SREG)
	PUSH		r24
	PUSH		r25
	PUSH		ZL
	PUSH		ZH

	;Load the pointer address into register Z.  As with all
	; 16 bit register accesses, you read the low byte first.
	LDS			ZL,						_pwm_events_low_ptr
	LDS			ZH,						_pwm_events_low_ptr+1

	; Move the pointer past the current OCR1B value
	ADIW		ZL,						0x02

#ifndef PWM_PORTA_UNUSED
	IN			r24,					_SFR_IO_ADDR(PORTA)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTA),	r24
#endif


#ifndef PWM_PORTB_UNUSED
	IN			r24,					_SFR_IO_ADDR(PORTB)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTB),	r24
#endif


#ifndef PWM_PORTC_UNUSED
	IN			r24,					_SFR_IO_ADDR(PORTC)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTC),	r24
#endif


#ifndef PWM_PORTD_UNUSED
	IN			r24,					_SFR_IO_ADDR(PORTD)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTD),	r24
#endif

	; Update pointer with next value in array
	STS			_pwm_events_low_ptr+1,	ZH
	STS			_pwm_events_low_ptr,	ZL

	;Load the new compare value into OCRnB.  As with all 
	; 16 bit register access, you write the high byte first.
	LD			r24,					Z+
	LD			r25,					Z+
	STS			OCR1BH,					r25
	STS			OCR1BL,					r24

	;Restore calling state
	POP			ZH
	POP			ZL
	POP			r25
	POP			r24
	OUT			_SFR_IO_ADDR(SREG),		r0
	POP			r0
	
	RETI