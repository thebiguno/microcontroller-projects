#include "WebStream.h"

#include <string.h>
#include <stdio.h>
#include <dcstring.h>

#define STATE_IDLE 0x00
#define STATE_READ_HEADERS 0x01
#define STATE_READ_ENTITY_IDENTITY 0x02
#define STATE_READ_ENTITY_CHUNKED 0x03
#define STATE_WRITE_HEADERS 0x10
#define STATE_WRITE_ENTITY_IDENTITY 0x20
#define STATE_WRITE_ENTITY_CHUNKED 0x30

using namespace digitalcave;

WebStream::WebStream(Stream* stream) :
	stream(stream),
	_state(STATE_IDLE),
	_available(0),
	_content_length(0),
	_status(0)
{}

WebStream::~WebStream() {
}

void WebStream::read_headers() {
	if (_state != STATE_IDLE) return;

	_state = STATE_READ_HEADERS;
    _method[0] = 0;
    _path[0] = 0;
    _status = 0;
    _content_length = 0;
    _available = 0;

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
			    // each header line has max length 254
				response(414, (char*) NULL);
				body("");
				return;
			} else if (b == '\n') {
				line[i] = 0;
				break;
			}
		}

		// empty line means headers are over and stream positioned at the body
		if (strcmp("\r\n", line) == 0) {
        	if (_state == STATE_READ_HEADERS) {
        	    _state = STATE_READ_ENTITY_IDENTITY;
        	}
			break;
		}

		if (l == 0) {
			if (strncmp("HTTP/", line, 5) == 0) {
				// a response
				// first line has the HTTP/1.1, status code, reason message
				strtok(line, " ");
				_status = atol(strtok(NULL, " "));
			} else {
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
				_available = atol(v);
				_content_length = _available;
			} else if (strcmp("Transfer-Encoding", k) == 0) {
				if (strcmp("chunked", v) == 0) {
					_state = STATE_READ_ENTITY_CHUNKED;
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
    _state = STATE_WRITE_HEADERS;

	stream->write(method);
	stream->write(' ');
	stream->write(uri);
	stream->write(' ');
	stream->write("HTTP/1.1\r\n");
	stream->flush();

	header("Content-Type", content_type);
}

void WebStream::request(const char* method, const char* uri, const char* content_type) {
	request((char *) method, (char*) uri, (char*) content_type);
}

void WebStream::response(uint16_t status, char* content_type) {
    _state = STATE_WRITE_HEADERS;

	char buf[4];
	snprintf(buf, 4, "%u", status);

	stream->write("HTTP/1.1 ");
	stream->write(buf);
	stream->write(' ');
	switch (status) {
		case 200: stream->write("OK"); break;
		case 201: stream->write("Created"); break;
		case 202: stream->write("Accepted"); break;
		case 203: stream->write("Non-Authoritative Information"); break;
		case 204: stream->write("No Content"); break;
		case 205: stream->write("Reset Content"); break;
		case 300: stream->write("Multiple Choices"); break;
		case 301: stream->write("Moved Permanently"); break;
		case 302: stream->write("Found"); break;
		case 303: stream->write("See Other"); break;
		case 305: stream->write("Use Proxy"); break;
		case 307: stream->write("Temporary Redirect"); break;
		case 400: stream->write("Bad Request"); break;
		case 402: stream->write("Payment Required"); break;
		case 403: stream->write("Forbidden"); break;
		case 404: stream->write("Not Found"); break;
		case 405: stream->write("Method Not Allowed"); break;
		case 406: stream->write("Not Acceptable"); break;
		case 408: stream->write("Request Timeout"); break;
		case 409: stream->write("Conflict"); break;
		case 410: stream->write("Gone"); break;
		case 411: stream->write("Length Required"); break;
		case 413: stream->write("Payload Too Large"); break;
		case 414: stream->write("URI Too Long"); break;
		case 415: stream->write("Unsupported Media Type"); break;
		case 417: stream->write("Expectation Failed"); break;
		case 500: stream->write("Internal Server Error"); break;
		default: stream->write("Unknown"); break;
	}
	stream->write("\r\n");
	stream->flush();

	header("Content-Type", content_type);
}

void WebStream::response(uint16_t status, const char* content_type) {
	response(status, (char*) content_type);
}

void WebStream::body(char* b) {
    if (_state != STATE_WRITE_HEADERS) return;
	_state = STATE_WRITE_ENTITY_IDENTITY;

	body_start(strlen(b));
	stream->write(b);
	stream->flush();
}
void WebStream::body(const char* b) {
	body((char*) b);
}

void WebStream::body_start(uint16_t len) {
    if (_state != STATE_WRITE_HEADERS) return;
	_state = STATE_WRITE_ENTITY_IDENTITY;

	if (len > 0) {
		char buf[6];
		snprintf(buf, 5, "%u", len);
		header("Content-Length", buf);
	}
	stream->write("\r\n");
	stream->flush();
	// now positioned at start of entity
}

void WebStream::body_chunked_start(uint8_t buflen) {
    if (_state != STATE_WRITE_HEADERS) return;
	_state = STATE_WRITE_ENTITY_CHUNKED;
	header("Transfer-Encoding", (char*) "chunked");
	stream->write("\r\n");
	stream->flush();

	chunk = new ArrayStream(buflen);
	// now positioned at start of entity
}

void WebStream::body_chunked_end() {
    if (_state != STATE_WRITE_ENTITY_CHUNKED) return;
	write_chunk();
	stream->write("0\r\n");
	delete chunk;
	chunk = NULL;
}

void WebStream::write_chunk() {
    if (_state != STATE_WRITE_ENTITY_CHUNKED) return;
	uint16_t sz = chunk->size();
	if (sz > 0) {
		char buf[6];
		snprintf(buf, 5, "%x", sz);
		stream->write(buf);
		stream->write("\r\n");
		stream->flush();
		uint8_t b = 0;
		for (uint8_t i = 0; i < sz; i++) {
			chunk->read(&b);
			stream->write(b);
		}
		stream->write("\r\n");
		stream->flush();
		chunk->clear();
	}
}

void WebStream::header(const char* k, char* v) {
    if (_state != STATE_WRITE_HEADERS) return;
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
	if (_state == STATE_READ_ENTITY_CHUNKED && _available == 0) {

		uint8_t b = 0;
		uint8_t i = 0;

		char line[8]; // FFFF\r\n0
		while (b != '\n') {
			uint8_t ok = stream->read(&b);
			if (ok) {
				line[i++] = b;
			}
		}
		line[i] = 0;
		_available = strtol(line, NULL, 16);
		if (_available == 0) {
			// discard the newline at the end of the stream
			stream->read(&b);
			stream->read(&b);

			_state = STATE_IDLE;
		}
	}
}

uint8_t WebStream::read(uint8_t* b) {
	read_headers();

	read_chunk_size();
	if (_available == 0) {
		return 0;
	}

	uint8_t ok = stream->read(b);
	if (ok) {
		_available--;

		if (_state == STATE_READ_ENTITY_CHUNKED && _available == 0) {
			uint8_t x[1];
			// read the crlf at the end of the data chunk
			stream->read(x);
			stream->read(x);
		}
	}
	return ok;
}

uint8_t WebStream::write(uint8_t b) {
	if (_state == STATE_WRITE_ENTITY_CHUNKED) {
		if (chunk->size() == 5) {
			write_chunk();
		}
		return chunk->write(b);
	} else if (_state == STATE_WRITE_ENTITY_IDENTITY) {
		return stream->write(b);
	}
    // not in the right state to write to the body
    return 0;
}

uint8_t WebStream::flush() {
	if (_state == STATE_WRITE_ENTITY_CHUNKED) {
		write_chunk();
	}
	return stream->flush();
}
