using namespace digitalcave;

ESP8622::ESP8622() {
	serial_init_b(115200);
	this.reset();
	this.setMux(1);
}

ESP8622::~ESP8622() {
}

// ESP8622 functions

void ESP8622::reset() {
	serial_write_s("AT+RST\n");
	_delay_ms(2000);
	while (serial.read());
}

/* 1 = station, 2 = AP, 3 = both */
void ESP8266::setMode(uint8_t mode) {
	this.serial.write("AT+CWMODE=");
	this.serial.write(48 + mode);
	this.serial.write('\r');
	this.serial.write('\n');
	this.response();
}

uint8_t ESP8266:response() {
	// <status>\r\n 		(optional)
	// \r\n					(blank line)
	// [OK|ERROR]\r\n		(status)
	uint8_t result;
	uint8_t i = 0;
	uint8_t b;
	while (true) {
		i++;
		b = serial.read();
		if (b == '\n' && i == 1) break; 	// \r\n
		else if (b == '\n') i = 0;			// <status>\r\n
	}
	b = serial.read();
	if (b == 'O') {
		result = 1;
	else {
		result = 0;
	}
	while (true) {
		b = serial.read();
		if (b == '\n') break; 				// OK|ERROR\r\n
	}
	return result;
}

/*
uint8_t ESP8622::getMode() {
	serial_write_s("AT+CWMODE?\n");
	for (uint8_t = 0; i < 8; i++) serial_read_c(); // +CWMODE:
	char mode = serial_read_c();
	for (uint8_t = 0; i < 3; i++) serial_read_c(); // \nOK
	return mode - 48;
}
*/
uint8_t ESP8622::join(char* ssid, char* password) {
	this.serial.write("AT+CWJAP=\"");
	this.serial.write(ssid);
	this.serial.write('\"');
	this.serial.write(',');
	this.serial.write('\"');
	this.serial.write(password);
	this.serial.write('\"');
	this.serial.write('\r');
	this.serial.write('\n');
	this.response();
}

void ESP8622::quit() {
	this.serial.write("AT+CWLAP\r\n");
	this.response();
}

void ESP8622::setMux() {
	this.serial.write("AT+CIPMUX=");
	this.serial.write(48 + 2);
	this.serial.write('\r');
	this.serial.write('\n');
	this.response();
}
Stream ESP8622::connect(char type, char* address, uint16_t port) {
	char[5] buf;
	sprintf(buf, "%d", port);
	
	uint8_t i = 0;
	for (uint8_t i = 0; i < 5; i++) {
		ESP8622Stream s = streams[i];
		if (s.available) {
			s.available = false;
			i = s.id;
			break;
		}
	}

	this.serial.write("AT+CIPSTART=");
	this.serial.write(48 + id);
	this.serial.write(',');
	this.serial.write('U' == type ? "\"UDP\"" : "\"TCP\"");
	this.serial.write(",\"")
	this.serial.write(address);
	this.serial.write("\",");
	this.serial.write(buf);
	this.serial.write('\r');
	this.serial.write('\n');
}

ESP8622Stream::ESP8266Stream(Stream serial, uint8_t id) {
	this.serial = serial;
	this.id = id;
	
	void ESP8622Stream::close(uint8_t id) {
		this.serial.write("AT+CIPCLOSE");
		this.serial.write('=');
		this.serial.write(48 + this.id);
		this.serial.write(',');
		this.serial.write('\r');
		this.serial.write('\n');
	}
	
	void ESP8622Stream::read
		
	}
	
	void ESP8622Stream::write(uint8_t b) {
		this.serial.write("AT+CIPSEND=");
		this.serial.write(48 + this.id);
		this.serial.write(',');
		this.serial.write(48 + 3);
		this.response();
		this.serial.read(); 		// >
		this.serial.read(); 		// sp
		this.serial.write(b);
		this.serial.write('\r');
		this.serial.write('\n');
	}
	
	void ESP8622Stream::write(char* msg) {
		uint8_t len = strlen(msg) + 2;
		char[4] buf;
		sprintf(buf, "%d", len);

		this.serial.write("AT+CIPSEND=");
		this.serial.write(48 + this.id);
		this.serial.write(',');
		this.serial.write(buf);
		this.response();
		this.serial.read(); // >
		this.serial.read(); // sp
		this.serial.write(msg);
		this.serial.write('\r');
		this.serial.write('\n');
	}
	
	void ESP8622Stream:println() {
		this.serial.write('\r');
		this.serial.write('\n');
	}
}