#ifndef ESP8266_H
#define ESP8266_H

#include <Stream.h>
#include <ArrayStream.h>
#include <stdint.h>
#include <stdlib.h>

namespace digitalcave {
	class ESP8266 {

		private:
			ArrayStream buffer;
			Stream* serial;
			uint8_t connected; // a bitmask of which streams are connected (1) and which are available (0)
			uint16_t length;    // the length of the current input
			uint8_t state;		// 0 = start; 1 = reading +IPD; 2 = reading length; 3 = reading remote ip / port; 4 = reading data
		
		public:
			ESP8266(Stream* serial, uint8_t bufferSize);
			~ESP8266();
		
			uint8_t join(char* ssid, char* password);
			void quit();
		
			uint8_t connect(char* address, uint16_t port);
			void close();
			
			void poll();
		private:
			void reset();
			void setMode();
			void setMux();
			uint8_t response();
	};
}

#endif
