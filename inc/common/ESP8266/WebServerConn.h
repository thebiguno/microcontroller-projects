#ifndef WEB_SERVER_CONN_H
#define WEB_SERVER_CONN_H

#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>

#include "ESP8266Socket.h"

namespace digitalcave {

	class WebServer;

	class WebServerConn : public Stream {
		private:
			WebServerConn(WebServer* server, ESP8266Socket* socket);
			~WebServerConn();

			WebServer* server;
			ESP8266Socket* socket;

			uint16_t _available = 0; // remaining bytes in the identity or chunk
			uint8_t flags = 0;  // [7..2] unused; [1] chunked; [0] fully read
			char _method[7];
			char _path[256];		// this is the storage for both path and query
			char* _query = NULL;
			uint16_t _content_length = 0; // the value of the content length header

			void read_headers();

			void send(uint16_t status, uint16_t len, char* content_type, const char* transfer_enc);
			/* Adds a header to the response */
			void send_header(const char* key, const char* value);
		public:

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

			char* method();
			char* path();
			char* query();
			uint16_t content_length();

			uint16_t available();
			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flush();
			uint8_t close();

			using Stream::reset;
			using Stream::write;

			friend class WebServer;
	};
}

#endif
