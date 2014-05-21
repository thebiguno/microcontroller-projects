#ifndef MP3_H
#define MP3_H

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

namespace digitalcave {
	
	class MP3 {
		private:
			//Play / stop
			volatile uint8_t *playPort;
			uint8_t playPin;
			//Volume up
			volatile uint8_t *upPort;
			uint8_t upPin;
			//Volume down
			volatile uint8_t *downPort;
			uint8_t downPin;
			
		public:
			/*
			 * Initializes the MP3 player.  Keeps the port / pin info, and
			 * sets volume to min, then increases to a sane level.
			 */
			MP3(volatile uint8_t *playPort, uint8_t playPin,
				volatile uint8_t *up, uint8_t upPin,
				volatile uint8_t *downPort, uint8_t downPin);
				
			/*
			 * Hits the play / stop button.
			 */
			void hitPlayPause();
			
			/*
			 * Hits the volume up button;
			 */
			void hitVolumeUp();
			
			/*
			 * Hits the volume down button;
			 */
			void hitVolumeDown();
	};
}

#endif