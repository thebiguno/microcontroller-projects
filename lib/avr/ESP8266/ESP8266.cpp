using namespace digitalcave;

ESP8622::ESP8622() {
	serial_init_b(115200);
	this.reset();
	this.setMux(0); // start in single connection mode
}

ESP8622::~ESP8622() {
}

// ESP8622 functions

void ESP8622::reset() {
	serial_write_s("AT+RST\n");
	_delay_ms(2000);
	while (serial_available()) serial_read_c();
}

/* 1 = station, 2 = AP, 3 = both */
void ESP8266::setMode(uint8_t mode) {
	serial_write_s("AT+CWMODE=");
	serial_write_c(48 + mode);
	this.println();
	while (serial_available()) serial_read_c(); // OK
}
uint8_t ESP8622::getMode() {
	serial_write_s("AT+CWMODE?\n");
	for (uint8_t = 0; i < 8; i++) serial_read_c(); // +CWMODE:
	char mode = serial_read_c();
	for (uint8_t = 0; i < 3; i++) serial_read_c(); // \nOK
	return mode - 48;
}

uint8_t ESP8622::joinAP(char* ssid, char* password) {
	serial_write_s("AT+CWJAP=\"");
	serial_write_s(ssid);
	serial_write_s("\",\"");
	serial_write_s(password);
	serial_write_s("\"\r\n");
	while (serial_available()) serial_read_c(); // OK or ERROR
}

void ESP8622::quitAP() {
	serial_write_s("AT+CWLAP\r\n");
}

/* 1 = single, 2 = multiple */
void ESP8622::setMux(uint8_t mux) {
	serial_write_s("AT+CIPMUX=");
	serial_write_c(48 + mux);
	this.println();
	while (serial_available()) serial_read_c(); // OK
	this.mux = mux;
}
void ESP8622::connect(uint8_t id, char type, char* address, uint16_t port) {
	char[5] buf;
	sprintf(buf, "%d", port);

	serial_write_s("AT+CIPSTART=");
	if (mux == 1) {
		serial_write_c(48 + id);
		serial_write_c(',');
	}
	serial_write_s('U' == type ? "\"UDP\"" : "\"TCP\"");
	serial_write_s(",\"")
	serial_write_s(address);
	serial_write_s("\",");
	serial_write_s(buf);
	this.println();
}
uint8_t ESP8622::status() {
	serial_write_s("AT+CIPSTATUS\r\n");
	// TODO
}
void ESP8622::close(uint8_t id) {
	serial_write_s("AT+CIPCLOSE");
	if (id < 5) {
		serial_write_c('=');
		serial_write_c(48 + id);
		serial_write_c(',');
	}
	this.println();
}
void ESP8622::send(uint8_t id, char* msg) {
	uint8_t len = strlen(msg);
	char[4] buf;
	sprintf(buf, "%d", len);

	serial_write_s("AT+CIPSEND=");
	if (mux == 1) {
		serial_write_c(48 + id);
		serial_write_c(',');
	}
	serial_write_s(buf);
	serial_write_c(',');
	serial_write_s(msg);
}

void ESP8622:println() {
	serial_write_c('\r');
	serial_write_c('\n');
}