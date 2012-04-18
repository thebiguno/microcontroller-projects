#ifndef RADIO_H
#define RADIO_H

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

namespace digitalcave {
	
	class Radio {
		private:
			//Reset info
			volatile uint8_t *resetPort;
			uint8_t resetPin;
			
		public:
			/*
			 * Initializes the radio tuner with the given pin used as reset.
			 */
			Radio(volatile uint8_t *resetPort, uint8_t resetPin);
				
			/*
			 * Set to given channel frequency
			 */
			void setChannel(uint16_t);
	};
}

#endif