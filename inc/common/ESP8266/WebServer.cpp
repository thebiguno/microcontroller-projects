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
				result.resp_status = 414;
				send_resp(&result);
				close();
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

uint8_t WebServer::send_resp(conn_t* conn) {
	char buf[5];
	itoa(conn->resp_status, buf, 10);

	wifi->write("HTTP/1.1 ");
	wifi->write(buf);
	wifi->write(' ');
	switch (conn->resp_status) {
		case 200: wifi->write("OK"); break;
		case 201: wifi->write("Created"); break;
		case 202: wifi->write("Accepted"); break;
		case 203: wifi->write("Non-Authoritative Information"); break;
		case 204: wifi->write("No Content"); break;
		case 205: wifi->write("Reset Content"); break;
		case 300: wifi->write("Multiple Choices"); break;
		case 301: wifi->write("Moved Permanently"); break;
		case 302: wifi->write("Found"); break;
		case 303: wifi->write("See Other"); break;
		case 305: wifi->write("Use Proxy"); break;
		case 307: wifi->write("Temporary Redirect"); break;
		case 400: wifi->write("Bad Request"); break;
		case 402: wifi->write("Payment Required"); break;
		case 403: wifi->write("Forbidden"); break;
		case 404: wifi->write("Not Found"); break;
		case 405: wifi->write("Method Not Allowed"); break;
		case 406: wifi->write("Not Acceptable"); break;
		case 408: wifi->write("Request Timeout"); break;
		case 409: wifi->write("Conflict"); break;
		case 410: wifi->write("Gone"); break;
		case 411: wifi->write("Length Required"); break;
		case 413: wifi->write("Payload Too Large"); break;
		case 414: wifi->write("URI Too Long"); break;
		case 415: wifi->write("Unsupported Media Type"); break;
		case 417: wifi->write("Expectation Failed"); break;
		case 500: wifi->write("Internal Server Error"); break;
		default: wifi->write("Unknown"); break;
	}
	wifi->write("\r\n");
	if (conn->resp_type != NULL) {
		send_header("Content-Type", conn->resp_type);
	}
	if (conn->resp_encoding != NULL) {
		send_header("Content-Encoding", conn->resp_encoding);
	}
	if (conn->resp_length != 0) {
		itoa(conn->resp_length, buf, 10);
		send_header("Content-Length", buf);
	}
	send_header("Server", "DigitalCave (ESP8266)");
	send_header("Connection", "close");

	wifi->write("\r\n");

	wifi->flush();

	return 1;
}

void WebServer::send_header(char* k, char* v) {
	wifi->write(k);
	wifi->write(": ");
	wifi->write(v);
	wifi->write("\r\n");
}

uint8_t WebServer::flush() {
	return wifi->flush();
}

uint8_t WebServer::close() {
	this->flush();
	return wifi->close();
}
