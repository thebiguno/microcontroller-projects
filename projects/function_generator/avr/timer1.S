#include <avr/io.h>
#include <avr/sfr_defs.h>

	.global TIMER1_OVF_vect
TIMER1_OVF_vect:														;[5] (service interrupt)
	PUSH		r30														;[2]
	PUSH		r31														;[2]
	IN			r30,						_SFR_IO_ADDR(SREG)			;[1]
	OUT			_SFR_IO_ADDR(GPIOR2),		r30							;[1] -- we save 2 clock cycles by using GPIOR2 rather than another push / pop to store SREG value
	
	LDI			ZH,							hi8(_data)					;[2] -- load the MSB of _data pointer
	IN			ZL,							_SFR_IO_ADDR(GPIOR0)		;[1] -- load the current index into _data
	LD			r31,						Z							;[2]
	OUT			_SFR_IO_ADDR(PORTD),		r31							;[1]
	
	INC			r30														;[1] -- increment address as ZL already is loaded
	OUT			_SFR_IO_ADDR(GPIOR0),		r30							;[1] -- store address

	IN			r30,						_SFR_IO_ADDR(GPIOR2)		;[1]
	OUT			_SFR_IO_ADDR(SREG),			r30							;[1]
	POP			r31														;[2]
	POP			r30														;[2]
	RETI																;[4]

	.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:														;[5] (service interrupt)
	;Yeah, normally you should do all the ISR protection stuff here.  We skip it
	; because we know exactly what is happening during the times that this ISR 
	; can be called, and because we do not set any SREG flags in here.
	STS			TCNT1H,						r1							;[2] set TCNT1 to 0
	STS			TCNT1L,						r1							;[2]
	SBI			_SFR_IO_ADDR(PORTD),		5							;[2] turn on bit 5 in PORTD
	RETI																;[4]

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:														;[5] (service interrupt)
	;Yeah, normally you should do all the ISR protection stuff here.  We skip it
	; because we know exactly what is happening during the times that this ISR 
	; can be called, and because we do not set any SREG flags in here.
	CBI			_SFR_IO_ADDR(PORTD),		5							;[2] turn off bit 5 in PORTD
	RETI																;[4]