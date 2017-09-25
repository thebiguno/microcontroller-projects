#include "ESP8266.h"

#include <ArrayStream.h>

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial, uint16_t rxtxBufferSize) :
	serial(serial),
	buffer(ArrayStream(rxtxBufferSize))
{
	at_reset();
	at_mode();
	at_cifsr();
	at_mux();
}

ESP8266::~ESP8266() {
	buffer.clear();
}

uint32_t ESP8266::address() {
	return this->addr;
}

void ESP8266::at_reset() {
	serial->write("AT+RST\r\n");
//	_delay_ms(2000);
	uint8_t b;
	while (serial->read(&b));
}

void ESP8266::at_mode() {
	serial->write("AT+CWMODE=1\r\n");
	at_response(0);
}

void ESP8266::at_mux() {
	serial->write("AT+CIPMUX=1\r\n");
	at_response(0);
}

void ESP8266::at_cifsr() {
	uint8_t x = 0;
	uint8_t b = 0;
	uint8_t i = 0;
	uint32_t addr = 0;
	char status[26]; // + CIFSR:xxx.xxx.xxx.xxx0
	serial->write("AT+CIFSR");
	if (at_response(status)) {
		while (b != 0x3a && i < 26) {
			b = status[i++];
		}
		while (b != 0 && i < 26) {
			b = status[i++];
			if (b == 0) {
				addr |= x;
				break;
			} else if (b == 0x2e) {
				addr |= x;
				addr = addr << 8;
			} else {
				x *= 10;
				x += (b - 0x30);
			}
		}
	}
}

uint8_t ESP8266::join(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write("\",\"");
	serial->write(password);
	serial->write("\"\r\n");
	return at_response(0);
}

void ESP8266::quit() {
	serial->write("AT+CWQAP\r\n");
	this->at_response(0);
}

uint8_t ESP8266::at_response(char* status) {
	// <status>\r\n 		(optional status line)
	// \r\n					    (blank line)
	// [OK|ERROR|ALREAY CONNECT|SEND OK]\r\n		(status line)
	uint8_t result;
	uint8_t i = 0;
	uint8_t b;
	// read the optional status line and the blank line
	while (1) {
		i++;
		serial->read(&b);
		if (b == '\r') {}
		else if (b == '\n' && i == 1) { break; }	// \r\n
		else if (b == '\n') { i = 0; }				// <status>\r\n
		else { if (status != 0) *status++ = b; }
	}
	if (status != 0) *status = 0;

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
	this->at_response(0);
}

void ESP8266::stop_server(uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	this->serial->write("AT+CIPSERVER=0,");
	this->serial->write(buf);
	this->serial->write("\r\n");
	this->at_response(0);
}

void ESP8266::open_tcp(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		this->serial->write("AT+CIPSTART=");
		this->serial->write(id + 0x30);
		this->serial->write(",\"TCP\",\"");
		this->serial->write(address);
		this->serial->write("\",");
		this->serial->write(buf);
		this->serial->write("\r\n");
		uint8_t status = this->at_response(0);
		if (status == 0) {
			this->id = id;
		}
	}
}

void ESP8266::open_ucp(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		this->serial->write("AT+CIPSTART=");
		this->serial->write(id + 0x30);
		this->serial->write(",\"UCP\",\"");
		this->serial->write(address);
		this->serial->write("\",");
		this->serial->write(buf);
		this->serial->write("\r\n");
		uint8_t status = this->at_response(0);
		if (status == 0) {
			this->id = id;
		}
	}
}

uint8_t ESP8266::close() {
	this->flush();
	this->serial->write("AT+CIPCLOSE=");
	this->serial->write(this->id + 0x30);
	this->serial->write("\r\n");
 	return this->at_response(0);
}

uint8_t ESP8266::read(uint8_t* b) {
	return buffer.read(b);
}

uint16_t ESP8266::accept() {
	this->flush();
	buffer.clear();

	uint16_t length = 0;
	uint8_t state = 0; // 1 = +ipd, 2 = id, 3 = len, 4 = data
	uint8_t b;
	position = 0;

	while (1) {
		if (serial->read(&b)) {
			switch (state) {
				case 0:
					if (b == '+') state = 1;
					break;
				case 1:
					if (b == ',') state = 2;
					break;
				case 2:
					if (b == ',') state = 3;
					else this->id = b - 0x30;
					break;
				case 3:
					if (b == ':') {
						state = 4;
					} else {
						length *= 10;
						length += b - 0x30;
					}
					break;
				default:
					// fill the buffer with the request body
					if (!buffer.isFull()) buffer.write(b);
					if (position++ == length) return length;
			}
		} else if (state == 0) {
			// nothing to read
			return 0;
		}
	}
}

uint8_t ESP8266::write(uint8_t b) {
	if (buffer.isFull()) flush();
	return buffer.write(b);
}

uint8_t ESP8266::flush() {
	if (buffer.size() == 0) return 1; // nothing to write

	uint8_t len = buffer.size();
	uint8_t b;
	char buf[5];
	sprintf(buf, "%d", len);

	serial->write("AT+CIPSEND=");
	serial->write(buf);
	serial->write("\r\n");
	serial->read(&b); 			// >
	serial->read(&b); 			// sp
	while (buffer.read(&b)) {
		serial->write(b);
	}
	return this->at_response(0);
}
