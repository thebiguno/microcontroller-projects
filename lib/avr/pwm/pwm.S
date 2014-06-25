#include <avr/io.h>

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:

	;Save calling state
	PUSH		ZL
	PUSH		ZH
	PUSH		r20
	PUSH		r21
	IN			r20,					_SFR_IO_ADDR(SREG)
	PUSH		r20

	;Load the pointer address into register Z.  As with all
	; 16 bit register accesses, you read the low byte first.
	LDI			ZL,					lo8(_pwm_events_low_ptr)
	LDI			ZH,					hi8(_pwm_events_low_ptr)

#ifndef PWM_PORTA_UNUSED
	;Load portA low mask and increment pointer
	LD			r20,					Z+
	
	; AND PORTA and PORTA Mask
	AND			r20,					_SFR_IO_ADDR(PORTA)
	OUT			_SFR_IO_ADDR(PORTA),	r20
#endif


#ifndef PWM_PORTB_UNUSED
	;Load portB low mask and increment pointer
	LD			r20,					Z+

	; AND PORTB and PORTB Mask
	AND			r20,					_SFR_IO_ADDR(PORTB)
	OUT			_SFR_IO_ADDR(PORTB),	r20
#endif


#ifndef PWM_PORTC_UNUSED
	;Load portC low mask and increment pointer
	LD			r20,					Z+

	; AND PORTC and PORTC Mask
	AND			r20,					_SFR_IO_ADDR(PORTC)
	OUT			_SFR_IO_ADDR(PORTC),	r20
#endif


#ifndef PWM_PORTD_UNUSED
	;Load portD low mask and increment pointer
	LD			r20,					Z+

	; AND PORTD and PORTD Mask
	AND			r20,					_SFR_IO_ADDR(PORTD)
	OUT			_SFR_IO_ADDR(PORTD),	r20
#endif

	;Load the new compare value into OCRnB.  As with all 
	; 16 bit register access, you write the high byte first.
	LD			r20,					Z+
	LD			r21,					Z+
	STS			OCR1BH,					r20
	STS			OCR1BL,					r21

	;Save register Z back to pointer address.  As with all 
	; 16 bit register access, you write the high byte first.
	STS			_pwm_events_low_ptr+1,	ZH
	STS			_pwm_events_low_ptr,	ZL

	;Restore calling state
	POP			r20
	OUT			_SFR_IO_ADDR(SREG),		r20
	POP			r21
	POP			r20
	POP			ZH
	POP			ZL
	
	RETI