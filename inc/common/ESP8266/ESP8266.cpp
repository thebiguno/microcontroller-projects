#include "ESP8266.h"

#include "ESP8266Socket.h"

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial) :
	serial(serial)
{
	at_reset();
	at_mode();
	addr = at_cifsr();
	at_cipmux();

	for (uint8_t i = 0; i < 5; i++) {
		ESP8266Socket s(this, 0);
		sockets[i] = &s;
	}
}

ESP8266::~ESP8266() {
}

uint32_t ESP8266::address() {
	return this->addr;
}

void ESP8266::at_reset() {
	serial->write("AT+RST\r\n");
	//_delay_ms(1000);
	uint8_t b;
	while (serial->read(&b)); // don't parse this garbage, just read until it's over
}

void ESP8266::at_mode() {
	serial->write("AT+CWMODE=1\r\n");
	while ('+' == at_response());
}

void ESP8266::at_cipmux() {
	serial->write("AT+CIPMUX=1\r\n");
	while ('+' == at_response());
}

uint32_t ESP8266::at_cifsr() {
	uint8_t x = 0;
	uint8_t b = 0;
	uint8_t i = 0;
	uint32_t result = 0;
	serial->write("AT+CIFSR");
	while('+' == at_response());
	if ('O' == status[0]) {
		// data = xxx.xxx.xxx.xxx
		while (b != 0 && i < 26) {
			b = data[i++];
			if (b == 0) {
				result |= x;
				break;
			} else if (b == 0x2e) {
				result |= x;
				result = result << 8;
			} else {
				x *= 10;
				x += (b - 0x30);
			}
		}
	}
	return result;
}

uint8_t ESP8266::join(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write("\",\"");
	serial->write(password);
	serial->write("\"\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

void ESP8266::quit() {
	serial->write("AT+CWQAP\r\n");
	while ('+' == at_response());
}

void ESP8266::start_server(uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+CIPSERVER=1,");
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
}

void ESP8266::stop_server(uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+CIPSERVER=0,");
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
}

void ESP8266::start_mdns(char* host_name, char* server_name, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+MDNS=1,");
	serial->write(host_name);
	serial->write(',');
	serial->write(server_name);
	serial->write(',');
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
}

void ESP8266::stop_mdns(char* host_name, char* server_name, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+MDNS=0,");
	serial->write(host_name);
	serial->write(',');
	serial->write(server_name);
	serial->write(',');
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
}

ESP8266Socket* ESP8266::accept() {
	while ('<' == at_response());
	for (uint8_t i = 0; i < 5; i++) {
		if (sockets[i]->flags & 0x2) {
			sockets[i]->flags &= ~0x2;
			return sockets[i];
		}
	}
	return NULL;
}

ESP8266Socket* ESP8266::open_tcp(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		serial->write("AT+CIPSTART=");
		serial->write(id + 0x30);
		serial->write(",\"TCP\",\"");
		serial->write(address);
		serial->write("\",");
		serial->write(buf);
		serial->write("\r\n");
		while ('+' == at_response());
		if ('O' == status[0]) {
			sockets[id]->open(0x0);
			return sockets[id];
		}
	}
	return 0;
}

ESP8266Socket* ESP8266::open_ucp(char* address, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		serial->write("AT+CIPSTART=");
		serial->write(id + 0x30);
		serial->write(",\"UCP\",\"");
		serial->write(address);
		serial->write("\",");
		serial->write(buf);
		serial->write("\r\n");
		while ('+' == at_response());
		if ('O' == status[0]) {
			sockets[id]->open(0x0);
			return sockets[id];
		}
	}
	return 0;
}

uint8_t ESP8266::at_cipclose(uint8_t id) {
	serial->write("AT+CIPCLOSE=");
	serial->write(id + 0x30);
	serial->write("\r\n");
 	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cipsend(uint8_t id, uint16_t len, Stream* stream) {
	if (len == 0) return 1; // nothing to write

	uint8_t b;
	char buf[5];
	sprintf(buf, "%d", len);

	serial->write("AT+CIPSEND=");
	serial->write(id + 0x30);
	serial->write(',');
	serial->write(buf);
	serial->write("\r\n");
	while ('>' == at_response());
	while (stream->read(&b)) {
		serial->write(b);
	}
	while ('+' == at_response());
	if ('E' == status[0]) {
		sockets[id]->close();
	}
	return 'S' == status[0];
}

// when parsing a command it will always be either a response to the last
// command issued OR asynchronous IPD
// returns 0 when no data was available
// returns '+' when a command response was read
// returns '<' when IP data was read
// returns '>' when ready to receive was read (for +cipsend* commands)
uint8_t ESP8266::at_response() {
	// +<command>:<data>\r\n 										(optional line)
	// \r\n																			(blank line)
	// [OK|ERROR|ALREAY CONNECT|SEND OK]\r\n		(status line)

	// steps
	// 0 start
	// 1 +
	// 2 command
	// 3 :
	// 4 data
	// 5 command:info line \r\n
	// 6 blank line \r\n
	// 7 status
	// 8 status \r\n

	// 1 >
	// 21 sp

	// IPD,id,len:data
	// 3 ,
	// 41 ,
	// 42 id
	// 43 ,
	// 44 len
	// 45 :
	// 46 data

	uint8_t id;
	uint16_t len;
	uint16_t i;
	uint8_t step = 0;
	uint8_t result;
	uint8_t b;

	char* command = this->command;
	char* data = this->data;
	char* status = this->status;

	while (1) {
		uint8_t ct = serial->read(&b);
		// command flow
		if (step == 0 && ct == 0) { result = 0; break; } // there was nothing available
		else if (step == 0 && b == '+') { step = 1; }
		else if (step == 0 && b == '>') { step = 21; } // break out to +cipsend flow
		else if (step == 0 && b == '\r') { step = 6; } // skip forward in command flow
		else if (step == 2 && b == ',')  { step = 41; } // break out to +ipd flow
		else if (step == 2 && b == ':')  { step = 4; }
		else if (step == 2) { *command++ = b; }
		else if (step == 3) { step = 4; }
		else if (step == 4 && b == '\r') { step = 5; }
		else if (step == 4) { *data++ = b; }
		else if (step == 5 && b == '\n') { step = 6; }
		else if (step == 6 && b == '\n') { step = 7; }
		else if (step == 7 && b == '\r') { step = 8; }
		else if (step == 7) { *status++ = b; }
		else if (step == 8 && b == '\n') { result = 1; break; } // don't read anything more

		// +cipsend flow
		else if (step == 21) { result = '>'; break; }

		// +ipd flow
		else if (step == 41) { step = 42; }
		else if (step == 42 && b == ',') { step = 43; }
		else if (step == 42) { id = b - 0x30; sockets[id]->open(0x3); step = 43; }
		else if (step == 43) { step = 44; }
		else if (step == 44 && b == ':') { len *= 10; len += b - 0x30; step = 45; }
		else if (step == 44) { len *= 10; len += b - 0x30; }
		else if (step == 45) { step = 46; }
		else if (step == 46 && i == len) { sockets[id]->input->write(b); result = 2; break; } // don't read anything more
		else if (step == 46) { sockets[id]->input->write(b); i++; }
		else { step = 0; } // give up and start over
	}
	// add null terminators
	if (command != 0) { *command++ = 0;}
	if (data != 0) { *data++ = 0; }
	if (status != 0) { *status++ = 0; }

	return result;
}
