#ifndef ESP8266_H
#define ESP8266_H

#include <Module.h>
#include <stdint.h>

namespace digitalcave {
	class ESP8266 {

	private:
		Stream serial;
		uint8_t connected; // a bitmask of which streams are connected (1) and which are available (0)
		
	public:
		ESP8266(Stream serial);
		~ESP8266();
		
		void join(char* ssid, char* password);
		void quit();
		
		Stream connect_tcp(char* address, uint16_t port);
		Stream connect_udp(char* address, uint16_t port);

	private:
		void reset();
		void setMode(uint8_t);
		void println();
	}
	
	class ESP8266Stream : public Stream {

	private:
		uint8_t id;
		
		ESP8266Stream(ESP8266 esp, uint8_t id);
		~ESP8266Stream();
	}
}

#endif
