#include "ESP8266Socket.h"
#include "ESP8266.h"

using namespace digitalcave;

ESP8266Socket::ESP8266Socket(ESP8266* wifi, uint8_t id) :
	wifi(wifi),
	_id(id),
	flags(0xff)
{}

ESP8266Socket::~ESP8266Socket() {
	close();
}

uint8_t ESP8266Socket::id() {
	return _id;
}

uint16_t ESP8266Socket::available() {
	return input->size();
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

void ESP8266Socket::open(uint8_t flags) {
	this->flags = flags;

	if (input == NULL) {
		input = new ArrayStream(1460);
	}
	if (output == NULL) {
		output = new ArrayStream(512);
	}
}

uint8_t ESP8266Socket::close() {
	if (flags == 0xff) return 0;
	flags = 0xff;
	flush();
	uint8_t result = wifi->at_cipclose(_id);
	delete input;
	delete output;
	return result;
}

uint8_t ESP8266Socket::is_closed() {
	return 0xff == flags;
}
