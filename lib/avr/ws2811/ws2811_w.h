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
 * Sends GRB values to a chain of WS2811 leds.
 * v a pointer to the first GRB tuple
 * s the number of GRB tuples
 */
void ws2811_set(const void *values) {
	cli();
	
	uint8_t low_val = WS2811_PORT & ~_BV(WS2811_PIN);
	uint8_t high_val = WS2811_PORT | _BV(WS2811_PIN);

	asm volatile( 
			// current byte being sent is in r0
			// address of the end of the array is in r16
			"            LDI r16, 180\n"						// load the byte count into r2 (1)
			"            LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)
			// bit 7
			"start_byte: OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a7\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a7:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 6
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a6\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a6:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 5
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a5\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a5:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 4
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a4\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a4:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 3
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a3\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a3:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 2
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a2\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a2:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 1
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a1\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a1:         NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"	// (5)													L H (5)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L L
			// bit 0
			"            OUT %[port], %[high]\n"				// drive the line high (1)								L L
			"            BRCS a0\n" 							// if carry is set, skip next instruction (1/2)			H H
			"            OUT %[port], %[low]\n"					// else drive the line low (1)							H H
			"a0:         LD r0, %a[ptr]+\n"						// load next byte into temp reg (2)						L H (2)
			"            DEC r16\n"								// decrements the byte count (1)						L H
			"            CPI r16, 0\n"							// compare byte count to 0 (1)							L H
			"            BREQ end\n"							// jump to the end if the array is done (1)				L H
			"            LSL r0\n"								// left shift to set carry flag with bit value (1)		L H
			"            NOP\n"									// (1)													L H
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
			"            JMP start_byte\n"						// start the next byte									L L
			
			"end:        NOP\n" "NOP\n"							// (2)													L H (2)
			"            OUT %[port], %[low]\n"					// drive the line low (1)								L H
	: // no outputs
	: // inputs
	[ptr]   "e" (values), 	// pointer to grb values
	[high]  "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[low]   "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[port]  "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
		);
	
	sei();
	_delay_us(50);  // hold the line low for 50 microseconds to send the reset signal.
}

#endif
