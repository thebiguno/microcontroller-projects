#include "ESP8266.h"

#include "ESP8266Socket.h"

#include <string.h>

using namespace digitalcave;

ESP8266::ESP8266(Stream* serial) :
	serial(serial)
{
	for (uint8_t id = 0; id < 5; id++) {
		sockets[id] = new ESP8266Socket(this, id);
	}
}

ESP8266::~ESP8266() {
	for (uint8_t id = 0; id < 5; id++) {
		delete sockets[id];
	}
}

void ESP8266::init() {
	at_rst();
	at_cwmode_cur_sta();
	at_cipmux(1);
	at_cwdhcp_cur_sta(1);
}

uint8_t ESP8266::at_rst() {
	serial->write("AT+RST\r\n");
	at_response('+');
	//_delay_ms(100);
	uint8_t b;
	while (serial->read(&b)); // don't parse this garbage, just read until it's over
	return 'O' == status[0];
}

uint8_t ESP8266::at() {
	serial->write("AT\r\n");
	at_response('+');
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwmode_cur_sta() {
	serial->write("AT+CWMODE=1\r\n");
	at_response('+');
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwdhcp_cur_sta(uint8_t en) {
	serial->write("AT+CWDHCP=1,");
	serial->write(en ? '1' : '0');
	serial->write("\r\n");
	at_response('+');
	return 'O' == status[0];
}

uint8_t ESP8266::at_cipmux(uint8_t en) {
	serial->write("AT+CIPMUX=");
	serial->write(en ? '1' : '0');
	serial->write("\r\n");
	at_response('+');
	return 'O' == status[0];
}

uint8_t ESP8266::at_cifsr(char* addr, char* mac) {
	serial->write("AT+CIFSR");
	at_response('+');
	if ('O' == status[0]) {
		// data: +CIFSR:STAIP,"xxx.xxx.xxx.xxx"\r\nCIFSR:STAMAC,"xx:xx:xx:xx:xx:x"\r\n
		char* a = strtok(data,"\r\n");
		char* m = strtok(NULL,"\r\n");

		if (addr != 0) {
			a = strtok(a, ",");
			a = strtok(NULL, ",");
			a = strtok(a, "\"");
			strcpy(addr, a);
		}

		if (mac != 0) {
			m = strtok(m, ",");
			m = strtok(NULL, ",");
			m = strtok(m, "\"");
			strcpy(mac, m);
		}
		return 1;
	}
	return 0;
}

uint8_t ESP8266::at_cwjap_cur(char* ssid, char* password) {
	serial->write("AT+CWJAP=\"");
	serial->write(ssid);
	serial->write("\",\"");
	serial->write(password);
	serial->write("\"\r\n");
	at_response('+');
	return 'O' == status[0];
}

uint8_t ESP8266::at_cwqap() {
	serial->write("AT+CWQAP\r\n");
	at_response('+');
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
	at_response('+');
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
	at_response('+');
	return 'O' == status[0];
}

ESP8266Socket* ESP8266::accept() {
	at_response('<');

	for (uint8_t a = 0; a < 5; a++) {
		id %= 5;
		ESP8266Socket* socket = sockets[id];
		id++;

		if (socket->is_server() && socket->available() > 0) {
			return socket;
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
		at_response('+');
		if ('O' == status[0]) {
			sockets[id]->openClient();
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
	at_response('+');
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
	at_response('>');
	for (uint16_t i = 0; i < len; i++) {
		stream->read(&b);
		serial->write(b);
	}
	uint8_t ok = at_cipsend_response();
	if (ok == 0) {
		sockets[id]->close();
	}

	return ok;
}

// when parsing a command it will always be either a response to the last
// command issued OR asynchronous IPD
// returns 0 when no data was available
// returns '+' when a command response was read
// returns '<' when IP data was read
// returns '>' when ready to receive was read (for +cipsend* commands)
void ESP8266::at_response(char until) {
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
	uint8_t max = 10;

	char* data = this->data;
	char* status = this->status;

	while (1) {
		uint8_t ct = serial->read(&b);

		// detect which flow to process
		if (max == 0) { d("max"); result = 0; abort(); break; }
		else if (ct == 0) { max--; } // TODO delay 20ms

		else if (step == 0 && j == 1 && b == '\n') { d("0 -> 1"); step = 1; } 	  // a blank line; normal command
		else if (step == 0 && j == 0 && b == '+') { d("0 -> 21"); step = 21; }		// +IPD
		else if (step == 0 && j == 0 && b == '>') { d("0 -> x; r -> >"); result = '>'; break; }		// +CIPSEND

		// normal command response
		else if (step == 1 && j == 0 && b == '\r') { }
		else if (step == 1 && j == 1 && b == '\n') { d("1 -> 2"); step = 2; }
		else if (step == 1) { d("1 -> 1, data"); *data++ = b; *data = 0; d(this->data); }
		else if (step == 2 && b == '\r') { }
		else if (step == 2 && b == '\n') { d("2 -> x, r -> +"); result = '+'; break; }
		else if (step == 2) { d("2 -> 2, status"); *status++ = b; *status = 0; d(this->status); }

		// +IPD
		else if (step == 21 && b == ',') { d("22 -> 22"); step = 22; }						// read +IPD,
		else if (step == 21) { }
		else if (step == 22 && b == ',') { d("22 -> 23, len = 0"); step = 23; len = 0; }	// read id,
		else if (step == 22) { d("22 -> 22, id"); id = b - 0x30; sockets[id]->openServer(); }
		else if (step == 23 && b == ':') { d("23 -> 23, i -> 0"); step = 24; i = 0; len--; }			// read len:
		else if (step == 23) { d("23 -> 23, len"); len *= 10; len += (b - 0x30); }
		else if (step == 24 && i == len) { d("24 -> x, r -> <"); sockets[id]->input->write(b); result = '<'; break; }
		else if (step == 24) { d("24 -> 24, i++"); sockets[id]->input->write(b); i++; }

		// give up and start over
		else { d("? -> 0"); step = 0; }

		if (b == '\n') { j = 0; }
		else { j++; }

		if (result == until) { d("until"); break; }
	}

	#ifdef ESP8266_DEBUG
	puts(this->data);
	puts(this->status);
	#endif
}

uint8_t ESP8266::at_cipsend_response() {
	uint8_t b = 0;
	uint8_t max = 10;
	char* status = this->status;
	*status = 0;

	while (1) {
		uint8_t ct = serial->read(&b);

		if (max == 0) { return 0; }
		else if (ct == 0) { max--; } // TODO delay 20ms
		else if (b == '\r') { }
		else if (b == '\n') { *status = 0; status = this->status; d(status); }
		else { *status++ = b; d("status"); }

		if (b == '\n' && this->status[0] == 'S') { return 1; }
		else if (b == '\n' && this->status[0] == 'E') { return 0; }
	}
}

void ESP8266::d(const char* debug) {
	#ifdef ESP8266_DEBUG
	puts(debug);
	#endif
}
