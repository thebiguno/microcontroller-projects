#ifndef ws2811_h
#define ws2811_h

#include <stdint.h>
#include <util/delay_basic.h>

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
void ws2811_set(const void *values, uint8_t array_size, uint8_t pin) {
	uint8_t sreg = SREG;
	cli();
	
	const uint8_t mask =_BV(pin);
	uint8_t low_val = WS2811_PORT & (~mask);
	uint8_t high_val = WS2811_PORT | mask;
	uint8_t size = array_size * sizeof(ws2811_t); // size in bytes

	// reset the controllers by pulling the data line low
	uint8_t bitcount = 7;
	WS2811_PORT = low_val;
	_delay_loop_1(107); // at 3 clocks per iteration, this is 320 ticks or 40us at 8Mhz

	// note: the labels in this piece of assembly code aren't very explanatory. The real documentation
	// of this code can be found in the spreadsheet ws2811@8Mhz.ods
	asm volatile(
			"start:  LDI %[bits], 7                          \n"
			"        LD __tmp_reg__, %a[dataptr]+            \n"
			"cont06: NOP                                     \n"
			"cont07: NOP                                     \n"
			"        OUT %[portout], %[downreg]              \n"
			"cont09: LSL __tmp_reg__                         \n"
			"s00:    OUT %[portout], %[upreg]                \n"
			"        BRCS skip03                             \n"
			"        OUT %[portout], %[downreg]              \n"
			"skip03: SUBI %[bits], 1                         \n"
			"        BRNE cont06                             \n"
			"        LSL __tmp_reg__                         \n"
			"        BRCC Lx008                              \n"
			"        LDI %[bits], 7                          \n"
			"        OUT %[portout], %[downreg]              \n"
			"        NOP                                     \n"
			"        OUT %[portout], %[upreg]                \n"
			"        SBIW %[bytes], 1                        \n"
			"        LD __tmp_reg__, %a[dataptr]+            \n"
			"        BRNE cont07                             \n"
			"        RJMP brk18                              \n"
			"Lx008:  OUT %[portout], %[downreg]              \n"
			"        LDI %[bits], 7                          \n"
			"        OUT %[portout], %[upreg]                \n"
			"        NOP                                     \n"
			"        OUT %[portout], %[downreg]              \n"
			"        SBIW %[bytes], 1                        \n"
			"        LD __tmp_reg__, %a[dataptr]+            \n"
			"        BRNE cont09                             \n"
			"brk18:  OUT %[portout], %[downreg]              \n"
			"        NOP                                     \n"
			"end:    OUT %[portout], %[upreg]                \n": /* no output */
	: /* inputs */
	[dataptr] "e" (values), 	// pointer to grb values
	[upreg]   "r" (high_val),	// register that contains the "up" value for the output port (constant)
	[downreg] "r" (low_val),	// register that contains the "down" value for the output port (constant)
	[bytes]   "w" (size),		// number of bytes to send
	[bits]    "d" (bitcount),       // number of bits/2
	[portout] "I" (_SFR_IO_ADDR(WS2811_PORT)) // The port to use
		);
		
	if (sreg & _BV(7)) sei();
}

#endif
