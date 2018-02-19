#ifndef WEB_STREAM_H
#define WEB_STREAM_H

#include <stdint.h>
#include <Stream.h>

namespace digitalcave {

	/* Contract for uniform request and response.
	 * Inspired by Elixir Plug.Conn but greatly simplified to only handle the basics
	 * When a request arrives, all request headers are read after which
	 * the content type, method, path, query, and data stream are available.
	 * After the request is read the response can be initiated using one of four
	 * mechanisms:
	 * - a fixed length string response is sent with send_identity
	 * - a fixed length binary response is sent with start_identity and subsequent calls to write
	 * - a chunked response is started with start_chunked
	 * - one or more string chunks is sent with send_chunk
	 * - one or more binary chunks is sent with start_chunk and subsequent calls to write
	 * - a chunked response is completed by calling close or by sending a 0 length chunk
	 * calls to write that exceed the size of the entity or chunk are ignored
	 * calling close for identity responses is optional
	 */
	class WebStream : public Stream {
		private:
			Stream* stream;

			uint8_t _state;  					// [7:2] unused [1] identity/chunked [0] headers/entity
			uint16_t _available; 			// remaining bytes in the identity or chunk
			char _method[7];
			char _path[256];					// this is the storage for both path and query
			char* _query;
			uint16_t _content_length; // the value of the content length header
			uint16_t _status;					// the status code

			void read_headers();
			void read_chunk_size();
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
			void send_header(const char* key, char* value);

			/* Sends a response to the client with identity transfer encoding. */
			void send_identity(char* body);
			void send_identity(const char* body);
			/* Sends a response to the client with identity transfer encoding to be written using the stream interface. */
			void start_identity(uint16_t len);

			/* Starts a chunked entity.*/
			void start_chunked();
			/* Writes a string body chunk */
			void send_chunk(char* chunk);
			void send_chunk(const char* chunk);
			/* Starts a binary body chunk to be written using the stream interface. */
			void start_chunk(uint16_t len);

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
			uint16_t read(uint8_t* a, uint16_t len);
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
