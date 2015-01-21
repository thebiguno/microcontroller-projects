#include <avr/io.h>

.macro  NOP_6
NOP
NOP
NOP
NOP
NOP
NOP
.endm
.macro  NOP_10
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
.endm

; We use the same code for both the interrupt vector as well as
; the loop; put it in a macro so that I don't write (and maintain)
; it twice.  13 cycles total.
.macro  NEXT_DDS_CYCLE
	;Load the address of the pointer to _data into register Z.  As with 
	; all 16 bit register accesses, you read the low byte first.
	LDS			ZL,						_data		;2 cycles
	LDS			ZH,						_data+1		;2 cycles

	;Load the value in _data_ptr into register 24
	LDS			r24,					_data_ptr	;2 cycles

	;Add the current data pointer offset.  Since _data is aligned to 256 bytes,
	; we can add just the low byte and not worry about overflow.
	ADD			ZL,						r24			;1 cycle
	
	;Increment the pointer, and store the index into data array back to _data_ptr
	INC			r24									;1 cycle
	STS			_data_ptr,				r24			;2 cycles
	
	LD			r24,					Z			;2 cycles
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
.endm

	.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:
	NEXT_DDS_CYCLE									;12 cycles
	STS			TCNT1H,	r1							;2 cycle
	STS			TCNT1L,	r1							;2 cycle
	RETI											;4 cycles

	.global output_dds_wave_2003hz
output_dds_wave_2003hz:
	loop_2003hz:
	NEXT_DDS_CYCLE									;13 cycles
	NOP_10											;24 cycles
	NOP_6
	NOP_6
	NOP
	NOP
	RJMP		loop_2003hz							;2 cycles
	
	.global output_dds_wave_2520hz
output_dds_wave_2520hz:
	loop_2520hz:
	NEXT_DDS_CYCLE									;13 cycles
	NOP_10											;16 cycles
	NOP_6
	RJMP		loop_2520hz							;2 cycles

	.global output_dds_wave_3125hz
output_dds_wave_3125hz:
	loop_3125hz:
	NEXT_DDS_CYCLE									;13 cycles
	NOP_10											;10 cycles
	RJMP		loop_3125hz							;2 cycles

	.global output_dds_wave_3906hz
output_dds_wave_3906hz:
	loop_3906hz:
	NEXT_DDS_CYCLE									;13 cycles
	NOP												;5 cycles
	NOP
	NOP
	NOP
	NOP
	RJMP		loop_3906hz							;2 cycles

	.global output_dds_wave_4882hz
output_dds_wave_4882hz:
	loop_4882hz:
	NEXT_DDS_CYCLE									;13 cycles
	NOP												;1 cycles
	RJMP		loop_4882hz							;2 cycles

	;Base case, as fast as possible with a 20MHz clock
	.global output_dds_wave_5208hz
output_dds_wave_5208hz:
	loop_5208hz:
	NEXT_DDS_CYCLE									;13 cycles
	RJMP		loop_5208hz							;2 cycles