#include <avr/io.h>

	.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:

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

	; Move the pointer past the current OCR0B value
	ADIW		ZL,						0x02

	IN			r24,					_SFR_IO_ADDR(PORTB)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTB),	r24

	IN			r24,					_SFR_IO_ADDR(PORTC)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTC),	r24

	IN			r24,					_SFR_IO_ADDR(PORTD)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTD),	r24

	IN			r24,					_SFR_IO_ADDR(PORTE)
	LD			r25,					Z+
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTE),	r24


	; Update pointer with next value in array
	STS			_pwm_events_low_ptr+1,	ZH
	STS			_pwm_events_low_ptr,	ZL

	;Load the new compare value into OCRnB.
	LD			r24,					Z+
	STS			OCR0B,					r24

	;Restore calling state
	POP			ZH
	POP			ZL
	POP			r25
	POP			r24
	OUT			_SFR_IO_ADDR(SREG),		r0
	POP			r0
	
	RETI
