#include "WebStream.h"

#include <string.h>
#include <stdio.h>
#include <dcstring.h>

using namespace digitalcave;

WebStream::WebStream(Stream* stream) :
	stream(stream),
	_state(0),
	_available(0),
	_content_length(0),
	_status(0)
{}

WebStream::~WebStream() {
}

void WebStream::read_headers() {
	if (_state > 0) return;
	_state |= 0x01;
	puts("* read_headers");

	char line[255];
	uint8_t l = 0;
	uint8_t b = 0;

	while (1) {
		uint8_t i = 0;
		while (1) {
			// read headers line by line
			uint8_t ok = stream->read(&b);
			if (ok) {
				line[i++] = b;
			}

			if (i == 255) {
				puts("* too long");
				response(414, (char*) NULL);
				send_identity("");
				return;
			} else if (b == '\n') {
				puts("* eol");
				line[i] = 0;
				break;
			}
		}
		puts("* line");
		puts(line);

		// empty line means headers are over and stream positioned at the body
		if (strcmp("\r\n", line) == 0) {
			//puts("* break");
			break;
		}

		if (l == 0) {
			if (strncmp("HTTP/", line, 5) == 0) {
				//puts("* response line");
				// a response
				// first line has the HTTP/1.1, status code, reason message
				strtok(line, " ");
				_status = atol(strtok(NULL, " "));
			} else {
				puts("* request line");
				// a request
				// first line has the method, path+query, HTTP/1.1
				strcpy(_method, strtok(line, " "));
				strcpy(_path, strtok(NULL, " "));
				strtok(_path, "?");
				_query = strtok(NULL, "");
			}
		} else {
			// header line
			char* k = strtok(line, ":");
			char* v = trim(strtok(NULL, "\n"));

			_content_length = 0;
			if (strcmp("Content-Length", k) == 0) {
				//puts("* content length");
				_available = atol(v);
				_content_length = _available;
			} else if (strcmp("Transfer-Encoding", k) == 0) {
				//puts("* transfer encoding");
				if (strcmp("chunked", v) == 0) {
					_state |= 0x2;
				}
			}
		}

		l++;
	}
}

uint16_t WebStream::status() {
	read_headers();
	return _status;
}
char* WebStream::method() {
	read_headers();
	return _method;
}
char* WebStream::path() {
	read_headers();
	return _path;
}
char* WebStream::query() {
	read_headers();
	return _query;
}
uint16_t WebStream::content_length() {
	read_headers();
	return _content_length;
}

void WebStream::request(char* method, char* uri, char* content_type) {
	stream->write(method);
	stream->write(' ');
	stream->write(uri);
	stream->write(' ');
	stream->write("HTTP/1.1\r\n");
	stream->flush();

	send_header("Content-Type", content_type);
}

void WebStream::request(const char* method, const char* uri, const char* content_type) {
	request((char *) method, (char*) uri, (char*) content_type);
}

void WebStream::response(uint16_t status, char* content_type) {
	char buf[4];
	snprintf(buf, 4, "%u", status);

	stream->write("HTTP/1.1 ");
	stream->write(buf);
	stream->write(' ');
	switch (status) {
		case 200: write("OK"); break;
		case 201: write("Created"); break;
		case 202: write("Accepted"); break;
		case 203: write("Non-Authoritative Information"); break;
		case 204: write("No Content"); break;
		case 205: write("Reset Content"); break;
		case 300: write("Multiple Choices"); break;
		case 301: write("Moved Permanently"); break;
		case 302: write("Found"); break;
		case 303: write("See Other"); break;
		case 305: write("Use Proxy"); break;
		case 307: write("Temporary Redirect"); break;
		case 400: write("Bad Request"); break;
		case 402: write("Payment Required"); break;
		case 403: write("Forbidden"); break;
		case 404: write("Not Found"); break;
		case 405: write("Method Not Allowed"); break;
		case 406: write("Not Acceptable"); break;
		case 408: write("Request Timeout"); break;
		case 409: write("Conflict"); break;
		case 410: write("Gone"); break;
		case 411: write("Length Required"); break;
		case 413: write("Payload Too Large"); break;
		case 414: write("URI Too Long"); break;
		case 415: write("Unsupported Media Type"); break;
		case 417: write("Expectation Failed"); break;
		case 500: write("Internal Server Error"); break;
		default: write("Unknown"); break;
	}
	stream->write("\r\n");
	stream->flush();

	send_header("Content-Type", content_type);
}

void WebStream::response(uint16_t status, const char* content_type) {
	response(status, (char*) content_type);
}

void WebStream::send_identity(char* body) {
	start_identity(strlen(body));
	stream->write(body);
	stream->flush();
}
void WebStream::send_identity(const char* body) {
	send_identity((char*) body);
}

void WebStream::start_identity(uint16_t len) {
	if (len > 0) {
		char buf[6];
		snprintf(buf, 5, "%u", len);
		send_header("Content-Length", buf);
	}
	stream->write("\r\n");
	stream->flush();
	// now positioned at start of entity
}

void WebStream::start_chunked() {
	send_header("Transfer-Encoding", (char*) "chunked");
	write("\r\n");
	flush();
	// now positioned at start of entity
}

void WebStream::send_chunk(char* chunk) {
	if (chunk == NULL) {
		start_chunk(0);
	} else {
		start_chunk(strlen(chunk));
		stream->write(chunk);
	}
	stream->write("\r\n");
	stream->flush();
}
void WebStream::send_chunk(const char* chunk) {
	send_chunk((char*) chunk);
}

void WebStream::start_chunk(uint16_t len) {
	char buf[6];
	snprintf(buf, 5, "%x", len);
	stream->write(buf);
	stream->write("\r\n");
	stream->flush();
}

void WebStream::send_header(const char* k, char* v) {
	if (k != NULL && v != NULL) {
		stream->write(k);
		stream->write(": ");
		stream->write(v);
		stream->write("\r\n");
		stream->flush();
	}
}

uint16_t WebStream::available() {
	return _available;
}

void WebStream::read_chunk_size() {
	if (_state & 0x02 && _available == 0) {
		puts("* read chunk size");

		uint8_t b = 0;
		uint8_t i = 0;
		// chunked
		// need to read a new chunk
		// read size header
		char line[8]; // FFFF\r\n0
		while (b != '\n') {
			uint8_t ok = stream->read(&b);
			if (ok) {
				line[i++] = b;
			}
		}
		line[i] = 0;
		puts("* chunk line");
		puts(line);
		_available = strtol(line, NULL, 16);
		if (_available == 0) {
			// discard the newline at the end of the stream
			stream->read(&b);
			stream->read(&b);

			// drop out of chunked mode
			_state &= ~0x02;
		}
	}
}

uint8_t WebStream::read(uint8_t* b) {
	read_headers();

	if (_state & 0x02) {
		// chunked
		read_chunk_size();

		if (_available == 0) {
			return 0;
		}
	}

	uint8_t ok = stream->read(b);
	if (ok) {
		_available--;
	}
	return ok;
}

uint16_t WebStream::read(uint8_t* a, uint16_t len) {
	read_headers();

	uint16_t result = 0;

	if (_state & 0x02) {
		puts("* chunked read");
		// chunked
		while (result < len) {
			read_chunk_size();
			if (_available == 0) {
				break;
			} else if (len > _available) {
				uint16_t sz = stream->read(a, _available);
				result += sz;
				_available -= sz;
				a += sz;
			} else {
				uint16_t sz = stream->read(a, len);
				result += sz;
				_available -= sz;
				a += sz;
			}

			if (_available == 0) {
				// discard the newline at the end of the chunk
				uint8_t b = 0;
				stream->read(&b);
				stream->read(&b);
			}
		}
	} else {
		if (len > _available) {
			len = _available;
		}
		result = stream->read(a, len);
	}

	return result;
}

uint8_t WebStream::write(uint8_t b) {
	return stream->write(b);
}

uint8_t WebStream::flush() {
	return stream->flush();
}
