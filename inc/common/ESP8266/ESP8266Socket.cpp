#include "ESP8266Socket.h"
#include "ESP8266.h"

using namespace digitalcave;

ESP8266Socket::ESP8266Socket(ESP8266* wifi, uint8_t id, uint8_t flags) :
	wifi(wifi),
	_id(id),
	flags(flags),
	input(new ArrayStream(1460)),
	output(new ArrayStream(512))
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

uint8_t ESP8266Socket::close() {
	flush();
	uint8_t result = wifi->at_cipclose(_id);
	wifi->close_socket(_id);
	return result;
}
