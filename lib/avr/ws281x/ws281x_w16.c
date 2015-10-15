#ifndef WS281X_PORT
#define WS281X_PORT PORTB
#endif

#ifndef WS281X_PIN
#define WS281X_PIN 2
#endif

#ifndef WS281X_COUNT
#define WS281X_COUNT 144
#endif

#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*
http://wavedrom.com/editor.html
{signal: [
  {name: 'inst', wave: '5.534.5534...34.....34.55.4...34.....3x',
       data: ['ldi','ld','out','nop','lsl','brcs','out','nop','out','nop','out','nop','lsl','brcs','nop','out','nop','out']},
  {name: 'wire', wave: 'xxxx1....0...........1.........0......1' },
  {name: 'time', wave: 'xxxx2....2...........2.........2......x', data: ['312.5ns','750ns','625ns','437.5ns'] },
  {name: 'data', wave: 'xxxx2................2................x', data: ['bit n (0)','bit n (1)']}
]}
*/

/*
 * Sends values to a chain of WS2811 or WS2812 leds.
 * values a pointer to the first RGB triple (WS2811) or GRB triple (WS2812)
 */
void ws281x_set(const void *values) {
	cli();
	
	uint8_t low_val = WS281X_PORT & ~_BV(WS281X_PIN);
	uint8_t high_val = WS281X_PORT | _BV(WS281X_PIN);

	asm volatile( 
			// current byte being sent is in r0
			// address of the end of the array is in r16
			"            PUSH r16\n"
			"            LDI r16, %[ct]\n"						// load the size into r2 (1)
			"start_byte: LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			// g bit 7 (1062.5 ns)
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g7:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g6:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g5:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g4:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g3:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g2:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g1:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// g bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS g0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"g0:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			
			// r bit 7 (1062.5 ns)
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r7:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r6:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r5:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r4:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r3:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r2:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r1:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// r bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS r0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"r0:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			
			// b bit 7 (1062.5 ns)
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b7:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b6:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b5:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b4:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b3:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b2:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b1:         NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			// b bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            BRCS b0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"b0:         NOP\n" "NOP\n"
			"            DEC r16\n"								// decrements the size count (1)
			"            CPI r16, 0\n"							// compare size count to 0 (1)
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            BREQ end\n"							// jump to the end if the array is done (1/2)
			"            JMP start_byte\n"						// start the next grb (3)
			
			"end:        NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"            POP r16\n"
	: // no outputs
	: // inputs
	[ct]    "M" (WS281X_COUNT),		// the number of leds
	[ptr]   "e" (values), 	// pointer to grb values
	[high]  "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[low]   "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[port]  "I" (_SFR_IO_ADDR(WS281X_PORT)) // The port to use
		);
	
	sei();
	_delay_us(16);  // hold the line low for 16 microseconds to send the reset signal.
}


