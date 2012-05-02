#ifndef MATRIX_H
#define MATRIX_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {
	/*
	 * C++ implementation of LED matrix display math library.  It tracks the current
	 * frame (consisting of 64 bytes; each byte consists of 4 bits for red level and
	 * 4 bits for green level).  There is a method to get the current byte, given a 
	 * row and duty cycle; this byte is then used to drive the actual board from the 
	 * calling code.
	 */
	class Matrix {
		private:
			uint8_t displayBuffer[64];			//The display buffer for the current frame
	
		public:
			/*
			 * Sets the current frame, copying it into the internal displayBuffer.  Frame
			 * should be a 64 byte array, with byte 0 representing row = 1,col = 1, byte 1 
			 * representing row = 1, col = 2, byte 8 representing row = 2, col = 1, etc.
			 * The high 4 bits for each byte are red, and the low 4 bits are green.
			 */
			void setFrame(uint8_t* frame);
			
			/*
			 * Returns the byte for driving the given red row, for the given duty cycle
			 */
			uint8_t getRedByte(uint8t_t row, uint8_t dutyCycle);
			
			/*
			 * Returns the byte for driving the given green row, for the given duty cycle
			 */
			uint8_t getGreenByte(uint8t_t row, uint8_t dutyCycle);
	} ;
}
#endif
