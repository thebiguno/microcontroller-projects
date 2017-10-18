#ifndef ESP8266_SOCKET_H
#define ESP8266_SOCKET_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	class ESP8266; // forward declaration

	/* A stream wrapper for an ESP8266 Wifi module.
	 * Requests and responses are fully buffered with a shared buffer.
	 * Provides single threaded access to channels which means you can't act
	 * as a client and server at the same time, or act as multiple servers
	 * at the same time.
	 */
	class ESP8266Socket : public Stream {

		private:
			ESP8266Socket(ESP8266* wifi, uint8_t id);
			~ESP8266Socket();

			ESP8266* wifi;
			uint8_t _id;
			uint8_t flags; // [7..2] unused; [1] acceptable; [0] server
			ArrayStream* input;
			ArrayStream* output;
			void open(uint8_t flags);
		public:
			uint8_t id();
			uint16_t available();
			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flush();
			uint8_t close();
			uint8_t is_closed();

			using Stream::reset;
			using Stream::write;

			friend class ESP8266;
	};
}

#endif
