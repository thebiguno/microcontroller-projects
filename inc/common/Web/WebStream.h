#ifndef WEB_STREAM_H
#define WEB_STREAM_H

#include <stdint.h>
#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	/* Contract for uniform request and response.
	 * Inspired by Elixir Plug.Conn but greatly simplified to only handle the basics
	 * When a request arrives, all request headers are read after which
	 * the content type, method, path, query, and data stream are available.
	 * After the request is read the response can be initiated using one of four
	 * mechanisms:
	 * - a fixed length entity is sent with body or body_start
	 * - a variable length entity is sent with body_chunked_start and ended with body_chunked_end
	 *
	 * calls to write that exceed the size of the entity or chunk are ignored
	 * calling close for identity responses is optional
	 */
	class WebStream : public Stream {
		private:
			Stream* stream;
			ArrayStream* chunk;

			uint8_t _state;  					// read:  [3:2] unused [1] identity/chunked [0] headers 1 / entity 0
												// write: [7:6] unused [5] identity/chunked [4] headers 1 / entity 0
			uint16_t _available; 			    // remaining bytes in the identity or chunk
			char _method[7];
			char _path[256];					// this is the storage for both path and query
			char* _query;
			uint16_t _content_length;           // the value of the content length header
			uint16_t _status;					// the status code

			void read_headers();
			void read_chunk_size();

			void write_chunk();
		public:
			WebStream(Stream* stream);
			~WebStream();

			/* Starts a request to the server. */
			void request(char* method, char* uri, char* content_type);
			void request(const char* method, const char* uri, const char* content_type);

			/* Starts a response to the client. */
			void response(uint16_t status, char* content_type);
			void response(uint16_t status, const char* content_type);

			/* Sends a header.  Must be called after request/response and before any
			 * of the entity sending methods. */
			void header(const char* key, char* value);

			/* Sends an entity with identity encoding. */
			void body(char* body);
			void body(const char* body);
			void body_start(uint16_t len);

			/* Starts an entity with chunked encoding. */
			void body_chunked_start(uint8_t buflen);
			void body_chunked_end();

			/* Gets the response status */
			uint16_t status();

			/* Gets the request method */
			char* method();
			/* Gets the request path */
			char* path();
			/* Gets the request query */
			char* query();
			/* Gets the request content length */
			uint16_t content_length();

			// implementation of virtual methods
			uint8_t read(uint8_t* b);
			uint8_t write(uint8_t b);

			uint16_t available();
			uint8_t flush();

			// base class default method implementations
			using Stream::read;
			using Stream::write;
			using Stream::reset;
	};
}

#endif
