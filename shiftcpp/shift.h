#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

class Shift {

	private:
		
		volatile uint8_t *enable_port;
		uint8_t enable_pin;
		volatile uint8_t *clear_port;
		uint8_t clear_pin;
		
	public:
		
		/*
		 * Constructs the Shift Register interface.  
		 * size is the number of registers in the chain.
		 * Since this class interfaces directly with the AVR SPI hardware it should only be instantiated once.
		 */
		Shift(uint8_t size);
	
		void setCallback(void (*callback)());
		
		/*
		 * Configures latching after the last byte has been written.
		 */
		void setLatch(volatile uint8_t *port, uint8_t pin);
	
		/*
		 * Configures output enable.
		 */
		void setEnable(volatile uint8_t *port, uint8_t pin);
	
		/*
		 * Configures clear.
		 */
		void setClear(volatile uint8_t *port, uint8_t pin);
	
		/*
		 * Shifts all the bytes in the byte array into the shift register chain, latching at the end if latch is configured.
		 */
		void shift(uint8_t b[]);
	
		/*
		 * Enables output, if OE is configured.
		 */	
		void enable();

		/*
		 * Disables output, if OE is configured.
		 */	
		void disable();
	
		/*
		 * Clears the register, if clear is configured.
		 */
		void clear();
};
#endif
