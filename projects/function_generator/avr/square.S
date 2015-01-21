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
	
	; For 500kHz we want a total of 13 cycles per half waveform
	.global output_square_wave_769khz
output_square_wave_769khz:
	LDI			r24, 0xFF
	loop_769khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_10											;9 cycles
	NOP_6
	NOP
	NOP
	NOP
	RJMP		loop_769khz							;2 cycles

	; For 1MHz wave we need to insert 6 NOPs, for a total of 10 cycles per half waveform.
	.global output_square_wave_1000khz
output_square_wave_1000khz:
	LDI			r24, 0xFF
	loop_1000khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_6											;6 cycles
	RJMP		loop_1000khz						;2 cycles

	; For 1.25MHz wave we need to insert 4 NOP, for a total of 8 cycles per half waveform.
	; The entire waveform takes 16 cycles (8 cycles for half)
	.global output_square_wave_1250khz
output_square_wave_1250khz:
	LDI			r24, 0xFF
	loop_1250khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP												;4 cycles
	NOP
	NOP
	NOP
	RJMP		loop_1250khz						;2 cycles

	; For 1.429MHz wave we need to insert 3 NOP, for a total of 7 cycles per half waveform.
	; The entire waveform takes 14 cycles (7 cycles for half)
	.global output_square_wave_1429khz
output_square_wave_1429khz:
	LDI			r24, 0xFF
	loop_1429khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP												;3 cycles
	NOP
	NOP
	RJMP		loop_1429khz						;2 cycles
	
	; For 1.66MHz wave we need to insert 2 NOP, for a total of 6 cycles per half waveform.
	; The entire waveform takes 12 cycles (6 cycles for half)
	.global output_square_wave_1666khz
output_square_wave_1666khz:
	LDI			r24, 0xFF
	loop_1666khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP												;2 cycles
	NOP
	RJMP		loop_1666khz						;2 cycles

	; For 2MHz wave we need to insert 1 NOP, for a total of 5 cycles per half waveform.
	; The entire waveform takes 10 cycles (5 cycles for half)
	.global output_square_wave_2000khz
output_square_wave_2000khz:
	LDI			r24, 0xFF
	loop_2000khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP												;1 cycle
	RJMP		loop_2000khz						;2 cycles


	; For 2.5MHz wave we are running as close to the wire as possible (at 20MHz).
	; The entire waveform takes 8 cycles (4 cycles for half)
	.global output_square_wave_2500khz
output_square_wave_2500khz:
	LDI			r24, 0xFF
	loop_2500khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	RJMP		loop_2500khz						;2 cycles
