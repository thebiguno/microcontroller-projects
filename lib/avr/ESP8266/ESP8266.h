#ifndef ESP8266_H
#define ESP8266_H

#include <Module.h>
#include <stdint.h>

namespace digitalcave {
	class ESP8266 {
		
	private:
		uint8_t mux = 0;
		
	public:
		ESP8266();
		~ESP8266();
		
		void reset();
		
		/* 1 = station, 2 = AP, 3 = both */
		void setMode(uint8_t mode);
		uint8_t getMode();
		/* connect to an access point */
		void joinAP(char* ssid, char* password);
		/* disconnect from an access point */
		void quitAP();
		/* 1 = single, 2 = multiple */
		void setMux(uint8_t mux);
		/* connect a socket; 
		   id is a value between 0 and 4; or ignored if in single mux mode
		   type is either U for UDP or T for TCP
		 */
		void connect(uint8_t id, char type, char* address, uint16_t port);
		/* 2 = got ip, 3 = connected, 4 = disconnected */
		uint8_t status();
		/* close a socket using multiple mux */
		void close(uint8_t id);
		/* send bytes using multiple mux */
		void send(uint8_t id, char* msg);
		
	private:
		void println();
}

#endif
