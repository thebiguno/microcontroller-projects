#include "ws2811.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef WS2811_PORT
#define WS2811_PORT PORTB
#endif

#ifndef WS2811_DDR
#define WS2811_DDR DDRB
#endif

#ifndef WS2811_PIN
#define WS2811_PIN PINB1
#endif

void __attribute__((noinline)) ws2811_set(struct ws2811_t *colors, uint8_t count) {
	// Set the pin to be an output driving low.
	WS2811_PORT &= ~_BV(WS2811_PIN);
	WS2811_DDR |= _BV(WS2811_PIN);

	cli();   // Disable interrupts temporarily because we don't want our pulse timing to be messed up.
	while (count--) {
		// Send a color to the LED strip.
		// The assembly below also increments the 'colors' pointer,
		// it will be pointing to the next color at the end of this loop.
		asm volatile(
			"ld __tmp_reg__, %a0\n"
			"rcall send_led_strip_byte%=\n"	// Send green component.
			"ld __tmp_reg__, %a0+\n"
			"rcall send_led_strip_byte%=\n"	// Send red component.
			"ld __tmp_reg__, %a0+\n"
			"rcall send_led_strip_byte%=\n"	// Send blue component.
			"rjmp led_strip_asm_end%=\n"	// Jump past the assembly subroutines.

			// send_led_strip_byte subroutine:  Sends a byte to the LED strip.
			"send_led_strip_byte%=:\n"
			"rcall send_led_strip_bit%=\n"	// Send most-significant bit (bit 7).
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"
			"rcall send_led_strip_bit%=\n"	// Send least-significant bit (bit 0).
			"ret\n"

			// send_led_strip_bit subroutine:  Sends single bit to the LED strip by driving the data line
			// high for some time.  The amount of time the line is high depends on whether the bit is 0 or 1,
			// but this function always takes the same time (2 us).
			"send_led_strip_bit%=:\n"
			"sbi %2, %3\n"					// Drive the line high.
			"rol __tmp_reg__\n"				// Rotate left through carry.

	#if F_CPU == 16000000
			"nop\n" "nop\n"
	#elif F_CPU == 20000000
			"nop\n" "nop\n" "nop\n" "nop\n"
	#else
	#error "Unsupported F_CPU"
	#endif

			"brcs .+2\n" "cbi %2, %3\n"		// If the bit to send is 0, drive the line low now.

	#if F_CPU == 16000000
			"nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
	#elif F_CPU == 20000000
			"nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
			"nop\n" "nop\n"
	#endif

			"brcc .+2\n" "cbi %2, %3\n"		// If the bit to send is 1, drive the line low now.

			"ret\n"
			"led_strip_asm_end%=: "
			: "=b" (colors)
			: "0" (colors),		 			// %a0 points to the next color to display
			  "I" (_SFR_IO_ADDR(WS2811_PORT)),	// %2 is the port register (e.g. PORTC)
			  "I" (WS2811_PIN)     			// %3 is the pin number (0-8)
	    ); // volatile
	} // while
	
	sei();		  // re-enable interrupts now that we are done.
	_delay_us(50);  // hold the line low for 50 microseconds to send the reset signal.
}