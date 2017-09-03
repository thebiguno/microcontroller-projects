#include "ESP8266.h"

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial, uint8_t rxtxBufferSize) :
	serial(serial),
	buffer(rxtxBufferSize)
{
	at_reset();
	at_mode();
	at_cifsr();
	at_mux();
}

ESP8266::~ESP8266() {
	free((void*) data);
}

void ESP8266::address() {
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
	uint8_t x;
	uint8_t b;
	uint8_t status[26]; // + CIFSR:xxx.xxx.xxx.xxx0
	serial->write("AT+CIFSR");
	if (at_response(&status)) {
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
				addr << 8;
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
	at_response(0);
}

uint8_t ESP8266::at_response(uint8_t* status) {
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

void ESP8266::open(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		this->serial->write("AT+CIPSTART=");
		this->serial->write(itoa(id));
		this->serial->write(",\"TCP\",\"");
		this->serial->write(address);
		this->serial->write("\",");
		this->serial->write(buf);
		this->serial->write("\r\n");
		uint8_t status = this->at_response(0);
		if (status == 0) {
			this->mask |= (1 << id);
			this->id = id;
		}
	}
}

uint8_t ESP8266::close() {
	this->serial->write("AT+CIPCLOSE=");
	this->serial->write(itoa(this->id));
	this->serial->write("\r\n");
	this->mask &= ~(1 << id);
 	this->at_response();
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
	buffer.clear();
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
					else this->id = atoi(b);
					break;
				case 3:
					if (b == ':') {
						state = 4;
					} else {
						length *= 10;
						length += atoi(b);
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
	return at_response(0);
}
