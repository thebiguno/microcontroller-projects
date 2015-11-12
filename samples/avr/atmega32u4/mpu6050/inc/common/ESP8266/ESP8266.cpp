#include "ESP8266.h"

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial, uint8_t rxBufferSize, uint8_t txBufferSize) : rxBuffer(rxBufferSize), txBuffer(txBufferSize) {
//	serial.setBaud(115200);
	reset();
	setMode();
	setMux();
}

ESP8266::~ESP8266() {
}

void ESP8266::reset() {
	serial->write("AT+RST\n");
//	_delay_ms(2000);
	uint8_t b;
	while (serial->read(&b));
}

void ESP8266::setMode() {
	serial->write("AT+CWMODE=");
	serial->write(48 + 1); // station mode
	serial->write('\r');
	serial->write('\n');
	response();
}
void ESP8266::setMux() {
	serial->write("AT+CIPMUX=");
	serial->write(48 + 0); // single connection mode
	serial->write('\r');
	serial->write('\n');
	response();
}

uint8_t ESP8266::join(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write('\"');
	serial->write(',');
	serial->write('\"');
	serial->write(password);
	serial->write('\"');
	serial->write('\r');
	serial->write('\n');
	return response();
}

void ESP8266::quit() {
	serial->write("AT+CWLAP\r\n");
	response();
}

uint8_t ESP8266::response() {
	// <status>\r\n 		(optional)
	// \r\n					(blank line)
	// [OK|ERROR]\r\n		(status)
	uint8_t result;
	uint8_t i = 0;
	uint8_t b;
	while (true) {
		i++;
		serial->read(&b);
		if (b == '\n' && i == 1) break; 	// \r\n
		else if (b == '\n') i = 0;			// <status>\r\n
	}
	serial->read(&b);
	if (b == 'O') {
		result = 1;
	}
	else {
		result = 0;
	}
	while (true) {
		serial->read(&b);
		if (b == '\n') break; 				// OK|ERROR\r\n
	}
	return result;
}

uint8_t ESP8266::connect(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);
	
	serial->write("AT+CIPSTART=");
//	serial->write(48 + id);
//	serial->write(',');
	serial->write("\"TCP\",\"");
	serial->write(address);
	serial->write("\",");
	serial->write(buf);
	serial->write('\r');
	serial->write('\n');
	return response();
}

void ESP8266::close() {
	serial->write("AT+CIPCLOSE=");
//	serial->write(48 + id);
//	serial->write(',');
	serial->write('\r');
	serial->write('\n');
	response();
}

uint8_t ESP8266::read(uint8_t* b) {
	return rxBuffer.read(b);
}

uint8_t ESP8266::read(uint8_t* a, uint8_t len) {
	return rxBuffer.read(a, len);
}

void ESP8266::poll() {
	uint8_t b;
	while (serial->read(&b)) {
		switch (state) {
			case 0:
				if (b == '+') {
					state = 1;
					length = 0;
				}
				break;
			case 1:
				if (b == ',') state = 2;
				break;
			case 2:
				if (b == ',') {
					state = 3;
				} else {
					length *= 10;
					length += b - 48;
				}
				break;
			case 3:
				if (b == ':') state = 4;
				break;
			default:
				rxBuffer.write(b);
		}
	}
}

uint8_t ESP8266::write(uint8_t b) {
	if (txBuffer.isFull()) {
		flush();
	}
	return txBuffer.write(b);
}

void ESP8266::flush() {
	uint8_t len = txBuffer.size();
	uint8_t b;
	char buf[3];
	sprintf(buf, "%d", len);

	serial->write("AT+CIPSEND=");
	serial->write(buf);
	response();
	serial->read(&b); 			// >
	serial->read(&b); 			// sp
	while (txBuffer.read(&b)) {
		serial->write(b);
	}
	serial->write('\r');
	serial->write('\n');
}