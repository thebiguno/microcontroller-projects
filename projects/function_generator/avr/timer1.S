#include <avr/io.h>
#include <avr/sfr_defs.h>

	.global TIMER1_OVF_vect
TIMER1_OVF_vect:														;[5] (service interrupt)
	PUSH		r30														;[2]
	PUSH		r31														;[2]
	IN			r30,						_SFR_IO_ADDR(SREG)			;[1]
	PUSH		r30														;[2]
	
	LDI			ZH,							hi8(_data)					;[2] -- load the MSB of _data pointer
	IN			ZL,							_SFR_IO_ADDR(GPIOR0)		;[1] -- load the current index into _data
	LD			r31,						Z							;[2]
	OUT			_SFR_IO_ADDR(PORTD),		r31							;[1]
	
	INC			r30														;[1] -- increment address as ZL already is loaded
	OUT			_SFR_IO_ADDR(GPIOR0),		r30							;[1] -- store address

	POP			r30														;[2]
	OUT			_SFR_IO_ADDR(SREG),			r30							;[1]
	POP			r31														;[2]
	POP			r30														;[2]
	RETI																;[4]

;	.global TIMER1_COMPA_vect
;TIMER1_COMPA_vect:														;[5] (service interrupt)
;	STS			TCNT1H,						r1							;[2] set TCNT1 to 0
;	STS			TCNT1L,						r1							;[2]
;	SBI			_SFR_IO_ADDR(PORTD),		5							;[2] turn on bit 5 in PORTD
;	RETI																;[4]

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:														;[5] (service interrupt)
	CBI			_SFR_IO_ADDR(PORTD),		5							;[2] turn off bit 5 in PORTD
	RETI																;[4]