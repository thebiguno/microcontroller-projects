#ifndef ESP8266_H
#define ESP8266_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>

#include "ESP8266Socket.h"

namespace digitalcave {

	/* A stream wrapper for an ESP8266 Wifi module.
	 * Requests and responses are fully buffered with a shared buffer.
	 * Provides single threaded access to channels which means you can't act
	 * as a client and server at the same time, or act as multiple servers
	 * at the same time.
	 */
	class ESP8266 {

		private:
			Stream* serial;
			ESP8266Socket* sockets[5];

			uint8_t id;
			char data[32];
			char status[8];

			void at_response(char until);
			uint8_t at_cipsend_response();

			uint8_t at_cipsend(uint8_t id, uint16_t len, Stream* stream);
			uint8_t at_cipclose(uint8_t id);
			ESP8266Socket* at_cipstart(const char* type, const char* addr, uint16_t port);

			void d(const char* debug);
		public:
			ESP8266(Stream* serial);
			~ESP8266();

			// helper

			void init();

			// basic

			uint8_t at();
			uint8_t at_rst();

			// wifi

			uint8_t at_cwmode_cur_sta();
			uint8_t at_cwdhcp_cur_sta(uint8_t en);
			uint8_t at_cwjap_cur(char* ssid, char* password);
			uint8_t at_cwqap();
			uint32_t at_cifsr(char* addr);
			uint8_t at_mdns(uint8_t en, char* host_name, char* server_name, uint16_t port);

			// IP

			uint8_t at_cipmux(uint8_t en);
			uint8_t at_cipserver(uint8_t en, uint16_t port);
			ESP8266Socket* at_cipstart_tcp(const char* addr, uint16_t port);
			ESP8266Socket* at_cipstart_udp(const char* addr, uint16_t port);
			ESP8266Socket* at_cipstart_ssl(const char* addr, uint16_t port);

			/* Read incoming data and selects the channel the data belongs to.
			 * Output buffer is flushed and input buffer will contain the data to read.
			 * Returns the id of the connection that was selected.
			 * A server should call this repeatedly to receive new requests.
			 */
			ESP8266Socket* accept();

			friend class ESP8266Socket;
	};
}

#endif
