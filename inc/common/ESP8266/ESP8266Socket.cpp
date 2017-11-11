#include "ESP8266Socket.h"
#include "ESP8266.h"

#include <string.h>

using namespace digitalcave;

ESP8266Socket::ESP8266Socket(ESP8266* wifi, uint8_t id) :
	wifi(wifi),
	_id(id)
{}

ESP8266Socket::~ESP8266Socket() {
	close();
}

uint8_t ESP8266Socket::id() {
	return _id;
}

uint16_t ESP8266Socket::available() {
	if (input == NULL) {
		puts("0");
		return 0;
	} else {
		puts("size");
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

void ESP8266Socket::open(uint8_t x) {
	flags = x;
	if (input == NULL) {
		input = new ArrayStream(1460);
	}
	if (output == NULL) {
		output = new ArrayStream(512);
	}
	puts("open");
}

uint8_t ESP8266Socket::close() {
	if (is_closed()) return 1;
	flush();
	uint8_t ok = wifi->at_cipclose(_id);
	flags = 0x00;
	delete input;
	delete output;
	return ok;
}

uint8_t ESP8266Socket::is_closed() {
	return (0x00 == flags);
}

uint8_t ESP8266Socket::is_client() {
	return (0x01 == flags);
}

uint8_t ESP8266Socket::is_server() {
	return (0x02 == flags);
}
