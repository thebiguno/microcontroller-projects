#include "ESP8266.h"

#include "ESP8266Socket.h"

#include <string.h>

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial) :
	serial(serial)
{
	for (uint8_t i = 0; i < 5; i++) {
		ESP8266Socket s(this, i);
		sockets[i] = &s;
	}
}

ESP8266::~ESP8266() {
}

void ESP8266::init() {
	at_rst();
	at_cwmode_cur_sta();
	at_cipmux(1);
	at_cwdhcp_cur_sta(1);
}

uint8_t ESP8266::at_rst() {
	serial->write("AT+RST\r\n");
	while('+' == at_response());
	//_delay_ms(100);
	uint8_t b;
	while (serial->read(&b)); // don't parse this garbage, just read until it's over
	return 'O' == status[0];
}

uint8_t ESP8266::at() {
	serial->write("AT\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwmode_cur_sta() {
	serial->write("AT+CWMODE=1\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwdhcp_cur_sta(uint8_t en) {
	serial->write("AT+CWDHCP=1,");
	serial->write(en ? '1' : '0');
	serial->write("\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cipmux(uint8_t en) {
	serial->write("AT+CIPMUX=");
	serial->write(en ? '1' : '0');
	serial->write("\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint32_t ESP8266::at_cifsr() {
	uint32_t result = 0;
	serial->write("AT+CIFSR");
	while('+' == at_response());
	if ('O' == status[0]) {
		// data: +CIFSR:STAIP,"xxx.xxx.xxx.xxx"\r\nCIFSR:STAMAC,"xx:xx:xx:xx:xx:x"\r\n
		char* s;
		s = strtok(data,"\r\n");
		s = strtok(s, ",");
		s = strtok(NULL, ",");
		s = strtok(s, "\".");

		while (s != NULL) {
			result = result << 8;
			result |= atoi(s);
			strtok(NULL, ".");
		}
	}
	return result;
}

uint8_t ESP8266::at_cwjap_cur(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write("\",\"");
	serial->write(password);
	serial->write("\"\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwqap() {
	serial->write("AT+CWQAP\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_cipserver(uint8_t en, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+CIPSERVER=");
	serial->write(en ? '1' : 0);
	serial->write(',');
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
}

uint8_t ESP8266::at_mdns(uint8_t en, char* host_name, char* server_name, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	serial->write("AT+MDNS=");
	serial->write(en ? '1' : '0');
	serial->write(',');
	serial->write(host_name);
	serial->write(',');
	serial->write(server_name);
	serial->write(',');
	serial->write(buf);
	serial->write("\r\n");
	while ('+' == at_response());
	return 'O' == status[0];
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

ESP8266Socket* ESP8266::at_cipstart(const char* type, const char* addr, uint16_t port) {
	char buf[5];
	sprintf(buf, "%d", port);

	for (int8_t id = 0; id < 5; id++) {
		serial->write("AT+CIPSTART=");
		serial->write(id + 0x30);
		serial->write(",\"");
		serial->write(type);
		serial->write("\",\"");
		serial->write(addr);
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
ESP8266Socket* ESP8266::at_cipstart_tcp(const char* address, uint16_t port) {
	return at_cipstart("TCP", address, port);
}

ESP8266Socket* ESP8266::at_cipstart_udp(const char* address, uint16_t port) {
	return at_cipstart("UDP", address, port);
}

ESP8266Socket* ESP8266::at_cipstart_ssl(const char* address, uint16_t port) {
	return at_cipstart("SSL", address, port);
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
	for (uint16_t i = 0; i < len; i++) {
		stream->read(&b);
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
	// request / response flow
	// ???\r\n								(command echo when ATE1)
	// \r\n										(blank line)
	// ???\r\n 								(data lines)
	// \r\n										(blank line)
	// [OK|ERROR|FAIL]\r\n		(status line)

	// status line format depends on command

	// inbound data flow
	// +IPD,id,len:data

	// outbound data flow
	// ???\r\n								(command echo when ATE1)
	// >\s
	// [ERROR|SEND OK]

	// steps
	// 0 start
	//   \r\n
	// 1 command data (may contain \r\n, may be blank)
	//   \r\n
	// 2 command status\r\n

	// 0  start
	// 11 >s
	// 12 command status\r\n

	// 0
	// 21 +IPD,
	// 22 id,
	// 23 len:
	// 24 data\r\n


	uint8_t id = 0;
	uint16_t len = 0;
	uint16_t i = 0; // position in the data stream
	uint16_t j = 0; // position in a line
	uint8_t step = 0;
	uint8_t result = 0;
	uint8_t b = 0;

	char* data = this->data;
	char* status = this->status;

	while (1) {
		uint8_t ct = serial->read(&b);
		if (b == '\n') { j = 0; }

		// detect which flow to process
		if (step == 0 && ct == 0) { result = 0; break; }
		else if (step == 0 && b == '\n' && j == 1) { step = 1; } 	// normal command
		else if (step == 0 && b == '+') { step = 21; }						// +IPD
		else if (step == 0 && b == '>') { step = 11; }						// +CIPSEND

		// normal command response
		else if (step == 1 && b == '\r' && j == 0) { }
		else if (step == 1 && b == '\n' && j == 1) { step = 2; }
		else if (step == 1) { *data++ = b; }
		else if (step == 2 && b == '\r' && j == 0) { }
		else if (step == 2 && b == '\n' && j == 1) { result = '+'; break; }
		else if (step == 2) { *status++ = b; }

		// +CIPSEND
		else if (step == 11 && b == ' ') { result = '>'; step = 12; }
		else if (step == 12 && b == '\r' && j == 0) { }
		else if (step == 12 && b == '\n' && j == 1) { break; }
		else if (step == 12) { *status++ = b; }

		// +IPD
		else if (step == 21 && b == ',') { step = 22; }						// read +IPD,
		else if (step == 22 && b == ',') { step = 23; len = 0; }	// read id,
		else if (step == 22) { id = b - 0x30; sockets[id]->open(0x3); }
		else if (step == 23 && b == ':') { step = 24; i = 0; }			// read len:
		else if (step == 23) { len *= 10; len += b - 0x30; }
		else if (step == 24 && i == len) { sockets[id]->input->write(b); result = '<'; break; }
		else if (step == 24) { sockets[id]->input->write(b); i++; }

		// give up and start over
		else { step = 0; }
	}
	// add null terminators
	if (data != 0) { *data = 0; }
	if (status != 0) { *status = 0; }

	return result;
}
