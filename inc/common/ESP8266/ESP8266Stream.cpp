#include "ESP8266Stream.h"
#include "ESP8266.h"

#include <string.h>

using namespace digitalcave;

const uint8_t CONNECTION_CLOSED = 0;
const uint8_t CONNECTION_CLIENT = 1;
const uint8_t CONNECTION_SERVER = 2;

ESP8266Stream::ESP8266Stream(ESP8266* wifi, uint8_t id) :
	wifi(wifi),
	_id(id),
	flags(CONNECTION_CLOSED),
	input(NULL),
	output(NULL)
{}

ESP8266Stream::~ESP8266Stream() {
	close();
}

uint8_t ESP8266Stream::id() {
	return _id;
}

uint16_t ESP8266Stream::available() {
	if (input == NULL) {
		return 0;
	} else {
		return input->size();
	}
}

uint8_t ESP8266Stream::read(uint8_t* b) {
	return input->read(b);
}

uint8_t ESP8266Stream::write(uint8_t b) {
	if (output->isFull()) flush();
	return output->write(b);
}

uint8_t ESP8266Stream::flush() {
	return wifi->at_cipsend(_id, output->size(), output);
}

void ESP8266Stream::open(uint8_t f) {
	flags = f;
	if (input == NULL) {
		input = new ArrayStream(1460);
	}
	if (output == NULL) {
		output = new ArrayStream(512);
	}
}
void ESP8266Stream::open_client() {
	open(CONNECTION_CLIENT);
}
void ESP8266Stream::open_server() {
	open(CONNECTION_SERVER);
}

uint8_t ESP8266Stream::close() {
	if (is_closed()) return 1;
	flags = CONNECTION_CLOSED;
	flush();
	delete input;
	delete output;
	input = NULL;
	output = NULL;
	uint8_t ok = wifi->at_cipclose(_id);
	return ok;
}

uint8_t ESP8266Stream::is_closed() {
	return (CONNECTION_CLOSED == flags);
}

uint8_t ESP8266Stream::is_client() {
	return (CONNECTION_CLIENT == flags);
}

uint8_t ESP8266Stream::is_server() {
	return (CONNECTION_SERVER == flags);
}
