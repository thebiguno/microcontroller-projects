#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include "ESP8266.h"

namespace digitalcave {
	struct request_t {
		char method[7];
		char uri[121];
	};

	/* In order to use this class, make the following calls:
	 * request_t request = server->accept();
	 * // fully read the request body
	 * server->status(200);
	 * // fully write the response body
	 * server->flush();
	 */
	class WebServer : public Stream {

		private:
			ESP8266* wifi;
			uint16_t status;
			request_t request;

		public:
			/* Consume the entire ESP8266 module for the task of being a webserver. */
			WebServer(ESP8266* wifi);
			~WebServer();

			/* Reads an incoming request */
			request_t accept();
			/* Writes the response header */
			void writeStatus(uint16_t status);

			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flushAndClose();

			using Stream::reset;
			using Stream::write;
	};
}

#endif
