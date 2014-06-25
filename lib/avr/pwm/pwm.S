#include <avr/io.h>

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:

	;Save calling state
	PUSH		ZL
	PUSH		ZH
	PUSH		r24
	PUSH		r25
	IN			r24,					_SFR_IO_ADDR(SREG)
	PUSH		r24

	;XOR PORTD0
	LDI			r25,					0x01
	IN			r24,					_SFR_IO_ADDR(PORTD)
	EOR			r24,					r25
	OUT			_SFR_IO_ADDR(PORTD),	r24
	
	;Load the pointer address into register Z.  As with all
	; 16 bit register accesses, you read the low byte first.
	LDS			ZL,					_pwm_events_low_ptr
	LDS			ZH,					_pwm_events_low_ptr+1

#ifndef PWM_PORTA_UNUSED
	;Load portA low mask and increment pointer
	LD			r24,					Z+
	IN			r25,					_SFR_IO_ADDR(PORTA)
	; PORTx &= port_mask_x
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTA),	r24
#endif


#ifndef PWM_PORTB_UNUSED
	;Load portB low mask and increment pointer
	LD			r24,					Z+
	IN			r25,					_SFR_IO_ADDR(PORTB)
	; PORTx &= port_mask_x
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTB),	r24
#endif


#ifndef PWM_PORTC_UNUSED
	;Load portC low mask and increment pointer
	LD			r24,					Z+
	IN			r25,					_SFR_IO_ADDR(PORTC)
	; PORTx &= port_mask_x
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTC),	r24
#endif


#ifndef PWM_PORTD_UNUSED
	;Load portD low mask and increment pointer
	LD			r24,					Z+
	IN			r25,					_SFR_IO_ADDR(PORTD)
	; PORTx &= port_mask_x
	AND			r24,					r25
	OUT			_SFR_IO_ADDR(PORTD),	r24
#endif

	;Load the new compare value into OCRnB.  As with all 
	; 16 bit register access, you write the high byte first.
	LD			r24,					Z+
	LD			r25,					Z+
	STS			OCR1BH,					r24
	STS			OCR1BL,					r25

	;Save register Z back to pointer address.  As with all 
	; 16 bit register access, you write the high byte first.
	STS			_pwm_events_low_ptr+1,	ZH
	STS			_pwm_events_low_ptr,	ZL

	;Restore calling state
	POP			r24
	OUT			_SFR_IO_ADDR(SREG),		r24
	POP			r25
	POP			r24
	POP			ZH
	POP			ZL
	
	RETI