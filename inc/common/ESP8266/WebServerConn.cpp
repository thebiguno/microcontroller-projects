#include "WebServerConn.h"
#include "WebServer.h"

#include <string.h>
#include <dcstring.h>

using namespace digitalcave;

WebServerConn::WebServerConn(WebServer* server, ESP8266Socket* socket) :
	server(server),
	socket(socket)
{}

WebServerConn::~WebServerConn() {
	server->conns[socket->id()] = NULL;
}

void WebServerConn::read_headers() {
	char line[255];
	uint8_t l = 0;
	uint8_t b;
	uint8_t i;

	while (1) {
			while (b != '\n' && i < 255) {
				// read headers line by line
				socket->read(&b);
				line[i++] = b;
			}
			line[i] = 0;

			if (i == 255 && l == 0) {
				// each line can be no longer than 255 chars
				send_identity(414, NULL, 0);
				close();
				return;
			}

			// empty line means headers are over and stream positioned at the body
			if (strcmp("\r\n", line)) {
				break;
			}

			if (_method[0] == 0) {
				// first line has the method, path and query
				strcpy(_method, strtok(line, " "));
				strcpy(_path, strtok(NULL, " "));
				strtok(_path, "?");
				_query = strtok(NULL, "");
			} else {
				// header lines
				char* k = strtok(line, ":");
				char* v = trim(strtok(NULL, "\n"));

				if (strcmp("Content-Length", k)) {
					_available = atoi(v);
					_content_length = _available;
				}
				else if (strcmp("Transfer-Encoding", k)) {
					if(strcmp("chunked", v)) { flags |= 0x2; }
				}
			}

			l++;
	}
}

char* WebServerConn::method() {
	return _method;
}
char* WebServerConn::path() {
	return _path;
}
char* WebServerConn::query() {
	return _query;
}
uint16_t WebServerConn::content_length() {
	return _content_length;
}

void WebServerConn::send_identity(uint16_t status, char* content_type, char* body) {
	start_identity(status, content_type, strlen(body));
	write(body);
	flush();
}

void WebServerConn::start_identity(uint16_t status, char* content_type, uint16_t len) {
	send(status, len, content_type, "identity");
}

void WebServerConn::start_chunked(uint16_t status, char* content_type) {
	send(status, 0, content_type, "chunked");
}

void WebServerConn::send_chunk(char* chunk) {
	start_chunk(strlen(chunk));
	write(chunk);
	write("\r\n");
	flush();
}

void WebServerConn::start_chunk(uint16_t len) {
	char buf[6];
	snprintf(buf, 5, "%x", len);
	write(buf);
	write("\r\n");
}

void WebServerConn::send(uint16_t status, uint16_t len, char* content_type, const char* transfer_enc) {
	char buf[4];
	snprintf(buf, 3, "%u", status);

	write("HTTP/1.1 ");
	write(buf);
	write(' ');
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
	write("\r\n");
	send_header("Transfer-Encoding", transfer_enc);
	//send_header("Content-Encoding", content_enc); //
	if (content_type != NULL) {
		send_header("Content-Type", content_type);
	}
	if (len != 0) {
		char buf[6];
		snprintf(buf, 5, "%u", len);
		send_header("Content-Length", buf);
	}
	send_header("Server", "DigitalCave (ESP8266)");
	send_header("Connection", "close");
	write("\r\n");
}

void WebServerConn::send_header(const char* k, const char* v) {
	write(k);
	write(": ");
	write(v);
	write("\r\n");
}

uint16_t WebServerConn::available() {
	return _available;
}

uint8_t WebServerConn::read(uint8_t* b) {
	uint8_t result = socket->read(b);
	if (result == 0 && _available > 0) {
		result = socket->read(b);
	}
	_available -= result;
	return result;
}

uint16_t WebServerConn::read(uint8_t* a, uint16_t len) {
	uint8_t b;
	uint8_t i;
	if (flags & 0x02) {
		// chunked
		if (_available == 0) {
			// need to read a new chunk
			// read size header
			char line[8]; // FFFF\r\n0
			while (b != '\n') {
				socket->read(&b);
				line[i++] = b;
			}
			line[i] = 0;
			_available = strtol(line, NULL, 16);
		}
	}

	if (len > _available) len = _available;
	uint16_t result = socket->read(a, len);
	if (result == 0 && _available > 0) {
		result = socket->read(a, len);
	}
	_available -= result;

	if (_available == 0 && flags & 0x02) {
		// discard the newline at the end of the chunk
		socket->read(&b);
		socket->read(&b);
	}
	return result;
}

uint8_t WebServerConn::write(uint8_t b) {
	return socket->write(b);
}

uint8_t WebServerConn::flush() {
	return socket->flush();
}

uint8_t WebServerConn::close() {
	flush();
	return socket->close();
}
