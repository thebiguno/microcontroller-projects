#include "WebServer.h"

using namespace digitalcave;

WebServer::WebServer(ESP8266* wifi) :
	wifi(wifi)
{
	wifi->close(5);
	wifi->start_server(80);
	request
}

WebServer::~WebServer() {
}

request_t WebServer::accept() {
	wifi->poll();

	uint16_t length = wifi->select();
	if (length == 0) return NULL;

	uint8_t b;
	uint8_t i;

	// method
	while (b != 0x20) {
		wifi->read(&b);
		if (i < 7) method[i++] = b;
	}
	method[i] = 0;

	// uri
	b = 0; i = 0;
	while (b != 0x20) {
		wifi->read(&b);
		if (i < 121) uri[i++] = b;
	}
	uri[i] = 0;

	// ignore protocol and all headers
	b = 0; i = 0;
	while (1) {
		wifi->read(&b);
		if (b == '\n') {
			if (i == 1) break;
			i = 0;
		} else {
			i++;
		}
	}

	// stream now positioned at the body
}

uint8_t WebServer::read(uint8_t* b) {
	return wifi->read(b);
}

uint16_t WebServer::read(uint8_t* a, uint16_t len) {
	return wifi->read(a, len);
}

uint8_t write(uint8_t b) {
	return wifi->write(b);
}

uint8_t writeHeader(uint16_t status) {
	char buf[4];
	sprintf(buf, "%d", status);

	wifi->write("HTTP/1.1 ");
	wifi->write();
	wifi->write(" OK\r\n");
	wifi->write("Connection: close\r\n\r\n");
}

void flushAndClose() {
	wifi->flush();
	wifi->close();
}
