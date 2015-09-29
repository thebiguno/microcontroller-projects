#ifndef ANALOG_H
#define ANALOG_H
#include <avr/io.h>

#define ANALOG_AVCC			1

/*
 * Reads the current analog values into given data buffer.  Buffer must be at 
 * least as long as count in analog_init.
 */
void analog_read_a(uint16_t *data);

/*
 * Returns the current analog value for the given pin.
 */
uint16_t analog_read_p(uint8_t index);
namespace digitalcave {
	class Analog {
		private:
		
		public:
			//Constructor
			Analog();
			
			uint8_t read(uint8_t pin);
	};
}
	
#endif
