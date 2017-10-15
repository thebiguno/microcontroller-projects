#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include "ESP8266.h"

namespace digitalcave {
	struct conn_t {
		// request fields
		char method[7];
		char path[256];		// this is the storage for both path and query
		char* query = NULL;
		uint16_t req_length = 0; // the value of the content length header
	};

	/* In order to use this class, make the following calls:
	 * request_t request = server->accept();
	 * // fully read the request body
	 * server->status(200);
	 * // fully write the response body
	 * server->flushAndClose();
	 */
	class WebServer : public Stream {

		private:
			ESP8266* wifi;
			ArrayStream buffer;
			uint16_t available; // remaining bytes in the identity or chunk
			uint8_t flags = 0;  // [7..2] unused; [1] chunked; [0] fully read

			void send(uint16_t status, uint16_t len, char* content_type, const char* transfer_enc);
			/* Adds a header to the response */
			void send_header(const char* key, const char* value);

		public:
			/* Consume the entire ESP8266 module for the task of being a webserver. */
			WebServer(ESP8266* wifi, uint16_t buffer_size);
			~WebServer();

			/* Reads incoming request headers */
			conn_t accept();
			/* Read additional data from a request */
			void accept(conn_t conn);

			/* Sends a response to the client with identity transfer encoding. */
			void send_identity(uint16_t status, char* content_type, char* body);
			/* Sends a response to the client with identity transfer encoding to be written using the stream interface. */
			void start_identity(uint16_t status, char* content_type, uint16_t len);

			/* Sends a response to the client with chunked transfer encoding */
			void start_chunked(uint16_t status, char* content_type);
			/* Writes a body chunk */
			void send_chunk(char* chunk);
			/* Starts a body chunk to be written using the steam interface. */
			void start_chunk(uint16_t len);

			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flush();
			uint8_t close();

			using Stream::reset;
			using Stream::write;
	};
}

#endif
