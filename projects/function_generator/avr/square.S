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

	.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
	IN			r24,	_SFR_IO_ADDR(PORTD)			;1 cycle
	COM			r24									;1 cycle
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	STS			TCNT1H,	r1							;2 cycle
	STS			TCNT1L,	r1							;2 cycle
	RETI											;4 cycles
 
	; For 500kHz we want a total of 20 cycles per half waveform
	.global output_square_wave_500khz
output_square_wave_500khz:
	LDI			r24, 0xFF
	loop_500khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_10											;16 cycles
	NOP_6
	RJMP		loop_500khz							;2 cycles

	; For 1MHz wave we need to insert 6 NOPs, for a total of 10 cycles per half waveform.
	.global output_square_wave_1000khz
output_square_wave_1000khz:
	LDI			r24, 0xFF
	loop_1000khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_6												;6 cycles
	RJMP		loop_1000khz						;2 cycles


	; For 2.5MHz wave we are running as close to the wire as possible (at 20MHz).
	; The entire waveform takes 8 cycles (4 cycles for half)
	.global output_square_wave_2500khz
output_square_wave_2500khz:
	LDI			r24, 0xFF
	loop_2500khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	RJMP		loop_2500khz						;2 cycles
