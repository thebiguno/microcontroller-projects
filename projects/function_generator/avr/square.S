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
.macro  NOP_100
LDI     r25, 47			; 1 cycle (only executed once)
NOP						; 1 cycle (start of loop)
SUBI    r25, 1			; 1 cycle
BRNE    .-6				; 2 cycle except for end of loop, then 1
.endm

.macro  NOP_1000
LDI     r25, 235		; 1 cycle (only executed once)
NOP						; 1 cycle (start of loop)
SUBI    r25, 1			; 1 cycle
BRNE    .-6				; 2 cycle except for end of loop, then 1

LDI     r25, 235		; 1 cycle (only executed once)
NOP						; 1 cycle (start of loop)
SUBI    r25, 1			; 1 cycle
BRNE    .-6				; 2 cycle except for end of loop, then 1
.endm


	; For 1kHz we want a total of 6000 cycles per half waveform
	.global output_square_wave_1khz
output_square_wave_1khz:
	LDI			r24, 0xFF
	loop_1khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_1000										;596 cycles
	NOP_1000
	NOP_1000
	NOP_1000
	NOP_100
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_1khz							;2 cycles

	; For 5kHz we want a total of 1200 cycles per half waveform
	.global output_square_wave_5khz
output_square_wave_5khz:
	LDI			r24, 0xFF
	loop_5khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_1000										;1196 cycles
	NOP_100
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_5khz							;2 cycles

	; For 10kHz we want a total of 600 cycles per half waveform
	.global output_square_wave_10khz
output_square_wave_10khz:
	LDI			r24, 0xFF
	loop_10khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_100											;596 cycles
	NOP_100
	NOP_100
	NOP_100
	NOP_100
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_10khz							;2 cycles

	
	; For 25kHz we want a total of 240 cycles per half waveform
	.global output_square_wave_25khz
output_square_wave_25khz:
	LDI			r24, 0xFF
	loop_25khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_100											;236 cycles
	NOP_100
	NOP_10
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_25khz							;2 cycles


	; For 50kHz we want a total of 120 cycles per half waveform
	.global output_square_wave_50khz
output_square_wave_50khz:
	LDI			r24, 0xFF
	loop_50khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_100											;116 cycles
	NOP_10
	NOP_6
	RJMP		loop_50khz							;2 cycles

	; For 100kHz we want a total of 60 cycles per half waveform
	.global output_square_wave_100khz
output_square_wave_100khz:
	LDI			r24, 0xFF
	loop_100khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_10											;56 cycles
	NOP_10
	NOP_10
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_100khz							;2 cycles

	; For 250kHz we want a total of 40 cycles per half waveform
	.global output_square_wave_250khz
output_square_wave_250khz:
	LDI			r24, 0xFF
	loop_250khz:
	OUT			_SFR_IO_ADDR(PORTD),	r24			;1 cycle
	COM			r24									;1 cycle
	NOP_10											;36 cycles
	NOP_10
	NOP_10
	NOP_6
	RJMP		loop_250khz							;2 cycles

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
