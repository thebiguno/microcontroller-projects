#include "ESP8266Socket.h"
#include "ESP8266.h"

#include <string.h>

using namespace digitalcave;

const uint8_t SOCKET_CLOSED = 0;
const uint8_t SOCKET_CLIENT = 1;
const uint8_t SOCKET_SERVER = 2;

ESP8266Socket::ESP8266Socket(ESP8266* wifi, uint8_t id) :
	wifi(wifi),
	_id(id),
	flags(SOCKET_CLOSED),
	input(NULL),
	output(NULL)
{}

ESP8266Socket::~ESP8266Socket() {
	close();
}

uint8_t ESP8266Socket::id() {
	return _id;
}

uint16_t ESP8266Socket::available() {
	if (input == NULL) {
		return 0;
	} else {
		return input->size();
	}
}

uint8_t ESP8266Socket::read(uint8_t* b) {
	return input->read(b);
}

uint8_t ESP8266Socket::write(uint8_t b) {
	if (output->isFull()) flush();
	return output->write(b);
}

uint8_t ESP8266Socket::flush() {
	return wifi->at_cipsend(_id, output->size(), output);
}

void ESP8266Socket::open(uint8_t f) {
	flags = f;
	if (input == NULL) {
		input = new ArrayStream(1460);
	}
	if (output == NULL) {
		output = new ArrayStream(512);
	}
}
void ESP8266Socket::openClient() {
	open(SOCKET_CLIENT);
}
void ESP8266Socket::openServer() {
	open(SOCKET_SERVER);
}

uint8_t ESP8266Socket::close() {
	if (is_closed()) return 1;
	flags = SOCKET_CLOSED;
	flush();
	delete input;
	delete output;
	input = NULL;
	output = NULL;
	uint8_t ok = wifi->at_cipclose(_id);
	return ok;
}

uint8_t ESP8266Socket::is_closed() {
	return (SOCKET_CLOSED == flags);
}

uint8_t ESP8266Socket::is_client() {
	return (SOCKET_CLIENT == flags);
}

uint8_t ESP8266Socket::is_server() {
	return (SOCKET_SERVER == flags);
}
