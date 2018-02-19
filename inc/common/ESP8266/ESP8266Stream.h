#ifndef ESP8266_STREAM_H
#define ESP8266_STREAM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	class ESP8266; // forward declaration

	/* A stream wrapper for an ESP8266 Wifi channel. */
	class ESP8266Stream : public Stream {

		private:
			ESP8266Stream(ESP8266* wifi, uint8_t id);
			~ESP8266Stream();

			ESP8266* wifi;
			uint8_t _id;

			uint8_t flags;
			ArrayStream* input;
			ArrayStream* output;

			void open(uint8_t flags);
			void open_client();
			void open_server();
		public:
			uint8_t id();
			uint16_t available();
			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flush();
			uint8_t close();
			uint8_t is_closed();
			uint8_t is_client();
			uint8_t is_server();

			using Stream::reset;
			using Stream::write;

			friend class ESP8266;
	};
}

#endif
