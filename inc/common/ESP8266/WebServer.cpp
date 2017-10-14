#include "WebServer.h"

#include <string.h>
#include <dcstring.h>

using namespace digitalcave;

WebServer::WebServer(ESP8266* wifi, uint16_t buffer_size) :
	wifi(wifi),
	buffer(ArrayStream(buffer_size))
{
	wifi->close();
	wifi->start_server(80);
}

WebServer::~WebServer() {
}

conn_t WebServer::accept() {
	char line[255];

	conn_t result;

	uint16_t length = wifi->accept();
	if (length == 0) return result;

  uint8_t l = 0;
	uint8_t b;
	uint8_t i;

	while (1) {
			while (b != '\n' && i < 255) {
				wifi->read(&b);
				line[i++] = b;
			}
			line[i] = 0;

			if (i == 255 && l == 0) {
				send_identity(414, NULL, 0);
				close();
				return result;
			}

			if (strcmp("\r\n", line)) {
				// stream now positioned at the body
				break;
			}

			if (result.method[0] == 0) {
				strcpy(result.method, strtok(line, " "));
				strcpy(result.path, strtok(NULL, " "));
				strtok(result.path, "?");
				result.query = strtok(NULL, "");
			} else {
				char* k = strtok(line, ":");
				char* v = trim(strtok(NULL, "\n"));

				if (strcmp("Content-Length", k)) {
					available = atoi(v);
					result.req_length = available;
				}
			}

			l++;
	}

	return result;
}

void WebServer::send_identity(uint16_t status, char* content_type, char* body) {
	start_identity(status, content_type, strlen(body));
	write(body);
	flush();
}

void WebServer::start_identity(uint16_t status, char* content_type, uint16_t len) {
	send(status, len, content_type, "identity");
}

void WebServer::start_chunked(uint16_t status, char* content_type) {
	send(status, 0, content_type, "chunked");
}

void WebServer::send_chunk(char* chunk) {
	start_chunk(strlen(chunk));
	write(chunk);
	write("\r\n");
	flush();
}

void WebServer::start_chunk(uint16_t len) {
	char buf[6];
	snprintf(buf, 5, "%x", len);
	write(buf);
	write("\r\n");
}

void WebServer::send(uint16_t status, uint16_t len, char* content_type, const char* transfer_enc) {
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

void WebServer::send_header(const char* k, const char* v) {
	write(k);
	write(": ");
	write(v);
	write("\r\n");
}

uint8_t WebServer::read(uint8_t* b) {
	uint8_t result = wifi->read(b);
	if (result == 0 && available > 0) {
		wifi->accept(); // try to read more
		result = wifi->read(b);
	}
	available -= result;
	return result;
}

uint16_t WebServer::read(uint8_t* a, uint16_t len) {
	if (len > available) len = available;
	uint16_t result = wifi->read(a, len);
	if (result == 0 && available > 0) {
		wifi->accept(); // try to read more
		result = wifi->read(a, len);
	}
	available -= result;
	return result;
}

uint8_t WebServer::write(uint8_t b) {
	return wifi->write(b);
}

uint8_t WebServer::flush() {
	return wifi->flush();
}

uint8_t WebServer::close() {
	this->flush();
	return wifi->close();
}
