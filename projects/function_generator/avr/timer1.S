#include <avr/io.h>
#include <avr/sfr_defs.h>

	.global TIMER1_OVF_vect
TIMER1_OVF_vect:														;[5] (service interrupt)
	PUSH		r30														;[2]
	PUSH		r31														;[2]
	IN			r30,						_SFR_IO_ADDR(SREG)			;[1]
	PUSH		r30														;[2]
	
	LDI			ZH,							hi8(_data)					;[2] -- load the MSB of _data pointer
	LDS			ZL,							_data_ptr					;[2] -- load the current index into _data
	LD			r31,						Z							;[2]
	OUT			_SFR_IO_ADDR(PORTD),		r31							;[1]
	
	LDS			r31,						_dds_increment				;[2] -- increment and update the data index
	ADD			r30,						r31							;[1]
	STS			_data_ptr,					r30							;[1]
	

	POP			r30														;[2]
	OUT			_SFR_IO_ADDR(SREG),			r30							;[1]
	POP			r31														;[2]
	POP			r30														;[2]
	RETI																;[4]



	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:														;[5] (service interrupt)
	CBI			_SFR_IO_ADDR(PORTB),		3							;[2] turn off bit 3 in PORTB
	RETI																;[4]