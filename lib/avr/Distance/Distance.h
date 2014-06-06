#ifndef DISTANCE_H
#define DISTANCE_H

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

namespace digitalcave {
	/*
	 * C++ implementation of distance sensor library.  Uses timer2 for measuring time, but turns it on
	 * and off within the read loop, so can be shared among multiple instances of this class.
	 */
	class Distance {
		private:
			volatile uint8_t *trigger_port;			//The PORT for the trigger
			uint8_t trigger_num;					//The pin number (on trigger_port) for the trigger
			volatile uint8_t *data_pin;				//The PIN for reading the response
			uint8_t data_num;						//The pin number (on data_pin) for the data line
	
		public:
			/*
			 * To initialize this, you must pass in references to the port and pin.  E.g. if you were 
			 * to use pins C5 and C4 for the trigger and data respectively, you would init with
			 * values:
			 * Distance(&PORTC, PORTC5, &PINC, PINC4);
			 */
			Distance(volatile uint8_t *trigger_port, uint8_t trigger_num, volatile uint8_t *data_pin, uint8_t data_num);
			
			/*
			 * Signals the distance sensor to ping, and reads the distance value.  Returns the distance
			 * in mm to the object.
			 */
			uint16_t read();
	} ;

}
#endif
