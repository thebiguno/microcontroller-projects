#ifndef ws2811_h
#define ws2811_h

#ifndef WS2811_PIN
#define WS2811_PIN 2
#endif

#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

typedef struct ws2811_t {
    uint8_t green;
    uint8_t red;
    uint8_t blue;
} ws2811_t;

/*
http://wavedrom.com/editor.html
{signal: [
  {name: 'inst', wave: '22.234..534..34.....534..5.4..34.....53x', 
   	data: ['ldi','ld','lsl','out','nop','brcs','out','nop','out','nop','lsl','out','nop','brcs','nop','out','nop','lsl','out']},
  {name: 'wire', wave: 'xxxxx1....0...........1........0.......1' },
  {name: 'time', wave: 'xxxxx2....2...........2........2.......x', data: ['312.5ns','750ns','562.5ns','500ns'] },
  {name: 'data', wave: 'xxxxx2................2................x', data: ['bit n (0)','bit n (1)']}
]}*/


/*
 * Sends values to a chain of WS2811 or WS2812 leds.
 * values a pointer to the first RGB triple (WS2811) or GRB triple (WS2812)
 * sz the number of triples
 */
void ws2811_set(const void *values, uint16_t sz) {
	cli();
	
	uint8_t low_val = WS2811_PORT & ~_BV(WS2811_PIN);
	uint8_t high_val = WS2811_PORT | _BV(WS2811_PIN);

	asm volatile( 
			// current byte being sent is in r0
			// address of the end of the array is in r16
			"            LDI r16, [sz]\n"						// load the size into r2 (1)
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 7 (1062.5 ns)
			"start_byte: OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// g bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n"
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			
			// r bit 7 (1062.5 ns)
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// r bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n"
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			
			// b bit 7 (1062.5 ns)
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a7\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a6\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a5\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a4\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a3\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a2\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a1\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n" "NOP\n"
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// b bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)
			"            NOP\n" "NOP\n" "NOP\n"
			"            BRCS a0\n" 							// if carry is set, skip next instruction (1/2)
			"            OUT %[port], %[low]\n"					// else drive the line low (1)
			"            DEC r16\n"								// decrements the size count (1)
			"            CPI r16, 0\n"							// compare size count to 0 (1)
			"            NOP\n"
			"            OUT %[port], %[low]\n"					// drive the line low (1)
			"            BREQ end\n"							// jump to the end if the array is done (1/2)
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			"            JMP start_byte\n"						// start the next grb (3)
			
			"end:        NOP\n" "NOP\n" "NOP\n"
			"            NOP\n" "NOP\n"
	: // no outputs
	: // inputs
	[ptr]   "e" (values), 	// pointer to grb values
	[sz]    "r" (sz),		// number of grb values
	[high]  "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[low]   "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[port]  "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
		);
	
	sei();
	_delay_us(6);  // hold the line low for 50 microseconds to send the reset signal.
}

#endif
