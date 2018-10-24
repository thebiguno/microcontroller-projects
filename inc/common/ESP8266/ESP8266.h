#ifndef ESP8266_H
#define ESP8266_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>

#include "ESP8266Stream.h"

namespace digitalcave {

	/* A stream wrapper for an ESP8266 Wifi module. */
	class ESP8266 {

		private:
			Stream* serial;
			ESP8266Stream* connections[5];

			uint8_t id;
			char data[128]; // enough for cifsr, but not cwlap
			char status[8];

			void at_response(char until);
			uint8_t at_cipsend_response();

			uint8_t at_cipsend(uint8_t id, uint16_t len, Stream* stream);
			uint8_t at_cipclose(uint8_t id);
			ESP8266Stream* at_cipstart(const char* type, const char* addr, uint16_t port);
            uint8_t at_cipserver(uint8_t ssl, uint8_t en, uint16_t port);

			void d(const char* debug);
		public:
			ESP8266(Stream* serial);
			~ESP8266();

			// helper

			void init();

			// basic

            /* Tests AT startup. */
			uint8_t at();
			/* Restarts the module. */
			uint8_t at_rst();

			// wifi

            /* Sets the Wifi mode to station. */
			uint8_t at_cwmode_cur_sta();
			/* Enables or disables DHCP. */
			uint8_t at_cwdhcp_cur_sta(uint8_t en);
			/* Connects to an access point. */
			uint8_t at_cwjap_cur(char* ssid, char* password);
			/* Disconnects from an access point. */
			uint8_t at_cwqap();
			/* Gets the local IP address and MAC address. */
			uint8_t at_cifsr(char* addr, char* mac);
			/* Enables or disables MDNS.
			 * The service name should start with _. */
			uint8_t at_mdns(uint8_t en, char* host_name, char* service_name, uint16_t port);

			// IP

            /* Enables or disables multiple connections. */
			uint8_t at_cipmux(uint8_t en);

            /* Starts a TCP server connection. */
			uint8_t at_cipserver_tcp(uint8_t en, uint16_t port);
            /* Starts a SSL server connection. */
			uint8_t at_cipserver_ssl(uint8_t en, uint16_t port);

			/* Returns the next available server stream, or NULL if there are no outstanding requests. */
			ESP8266Stream* accept();
			/* Calls the function with the next available server stream, then closes the stream. */
			void accept(void (* f) (Stream*));

            /* Starts a TCP client connection. */
			ESP8266Stream* at_cipstart_tcp(const char* addr, uint16_t port);
			/* Starts a UDP client connection. */
			ESP8266Stream* at_cipstart_udp(const char* addr, uint16_t port);
			/* Starts a SSL client connection. */
			ESP8266Stream* at_cipstart_ssl(const char* addr, uint16_t port);

			/* Calls the function with a new TCP client stream, then closes the stream. */
			void at_cipstart_tcp(const char* addr, uint16_t port, void (* f) (Stream*));
			/* Calls the function with a new UDP client stream, then closes the stream. */
			void at_cipstart_udp(const char* addr, uint16_t port, void (* f) (Stream*));
			/* Calls the function with a new SSL client stream, then closes the stream. */
			void at_cipstart_ssl(const char* addr, uint16_t port, void (* f) (Stream*));

			friend class ESP8266Stream;
	};
}

#endif
