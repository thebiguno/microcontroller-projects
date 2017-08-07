#include "ESP8266.h"

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial, uint8_t txBufferSize) :
	serial(serial),
	txBuffer(txBufferSize)
{
	at_reset();
	at_mode();
	at_mux();
}

ESP8266::~ESP8266() {
	free((void*) data);
}

void ESP8266::at_reset() {
	serial->write("AT+RST\r\n");
//	_delay_ms(2000);
	uint8_t b;
	while (serial->read(&b));
}

void ESP8266::at_mode() {
	serial->write("AT+CWMODE=1\r\n");
	at_response();
}

void ESP8266::at_mux() {
	serial->write("AT+CIPMUX=1\r\n");
	at_response();
}

uint8_t ESP8266::join(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write("\",\"");
	serial->write(password);
	serial->write("\"\r\n");
	return at_response();
}

void ESP8266::leave() {
	serial->write("AT+CWLAP\r\n");
	at_response();
}

uint8_t ESP8266::at_response() {
	// <status>\r\n 		(optional status line)
	// \r\n					    (blank line)
	// [OK|ERROR|ALREAY CONNECT|SEND OK]\r\n		(required status line)
	uint8_t result;
	uint8_t i = 0;
	uint8_t b;
	// read and ignore the optional status line and the blank line
	while (1) {
		i++;
		serial->read(&b);
		if (b == '\n' && i == 1) break; 	// \r\n
		else if (b == '\n') i = 0;				// <status>\r\n
	}
	// use the first char of the status to determine the result
	serial->read(&b);
	if (b == 'O' || b == 'S') {					// OK, SEND OK
		result = 1;
	} else if (b == 'E' || b == 'A') {	// ERROR, ALREADY CONNECT
		result = 0;
	} else {
		result = 0;
	}
	// read ignore the rest of the required status line
	while (1) {
		serial->read(&b);
		if (b == '\n') break; 						// <status>\r\n
	}
	return result;
}

void ESP8266::start_server(uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	this->serial->write("AT+CIPSERVER=1,");
	this->serial->write(buf);
	this->serial->write("\r\n");
	this->at_response();
}

void ESP8266::stop_server(uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	this->serial->write("AT+CIPSERVER=0,");
	this->serial->write(buf);
	this->serial->write("\r\n");
	this->at_response();
}

int8_t ESP8266::open(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		this->serial->write("AT+CIPSTART=");
		this->serial->write(48 + id);
		this->serial->write(",\"TCP\",\"");
		this->serial->write(address);
		this->serial->write("\",");
		this->serial->write(buf);
		this->serial->write("\r\n");
		uint8_t status = this->at_response();
		if (status == 0) {
			this->id = id;
			free((void*) data);
			data = NULL;
			txBuffer.clear();
			return id;
		}
	}
	return -1;
}

uint8_t ESP8266::close(uint8_t id) {
	this->serial->write("AT+CIPCLOSE=");
	this->serial->write(48 + id);
	this->serial->write("\r\n");
 	return this->at_response();
}

uint8_t ESP8266::read(uint8_t* b) {
	if (position == length) {
		free((void*) data);
		data = NULL;
		return 0;
	}
	*b = data[position++];
	return 1;
}

void ESP8266::select(uint8_t id) {
	this->id = id;
	this->flush();
	free((void*) data);
	data = NULL;
	txBuffer.clear();
}

uint8_t ESP8266::select() {
	this->flush();
	free((void*) data);
	data = NULL;
	txBuffer.clear();

	uint8_t state = 0; // 0 = ipd, 1 = id, 2 = len, 3 = data
	uint8_t b;
	position = 0;

	this->serial->write("+IPD\r\n");
	while (1) {
		if (serial->read(&b)) {
			switch (state) {
				case 0:
					if (b == ',') state = 1;
					break;
				case 1:
					if (b == ',') state = 2;
					else this->id = b - 48;
					break;
				case 2:
					if (b == ':') {
						state = 3;
						data = (uint8_t*) malloc(length);
					} else {
						length *= 10;
						length += b - 48;
					}
					break;
				default:
					data[position++] = b;
					if (position == length) {
						position = 0;
						return this->id;
					}
			}
		}
	}
}

uint8_t ESP8266::write(uint8_t b) {
	if (txBuffer.isFull()) flush();
	return txBuffer.write(b);
}

uint8_t ESP8266::flush() {
	if (txBuffer.size() == 0) return 1; // nothing to write

	uint8_t len = txBuffer.size();
	uint8_t b;
	char buf[3];
	sprintf(buf, "%d", len);

	serial->write("AT+CIPSEND=");
	serial->write(buf);
	serial->write("\r\n");
	serial->read(&b); 			// >
	serial->read(&b); 			// sp
	while (txBuffer.read(&b)) {
		serial->write(b);
	}
	return at_response();
}
