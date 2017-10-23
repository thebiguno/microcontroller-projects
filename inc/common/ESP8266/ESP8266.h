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

			uint32_t addr;

			char command[16];
			char data[16];
			char status[16];

			void poll();						// process whatever is in the serial receive buffer
			void at_reset();				// send AT+RST
			void at_mode();					// send AT+CWMODE=1 to configure station mode
			uint32_t at_cifsr();		// send AT+CIFSR to lease an IP address
			void at_cipmux();				// send AT+CIPMUX=1 to configure multiple connection
			uint8_t at_cipclose(uint8_t id);
			uint8_t at_cipsend(uint8_t id, uint16_t len, Stream* stream);
			uint8_t at_response();	// handle the responses for AT commands
		public:
			ESP8266(Stream* serial);
			~ESP8266();

			/* Join an access point */
			uint8_t join(char* ssid, char* password);
			/* Quit an access point */
			void quit();
			/* Return the current IP address */
			uint32_t address();

			/* Start a server connection listener. */
			void start_server(uint16_t port);
			/* Stop a server connection listener.  */
			void stop_server(uint16_t port);

			/* Advertices a service with mDNS.
			 * service will be available at host_name.local
			 * server_name should be from http://dns-sd.org/ServiceTypes.html
			 */
			void start_mdns(char* host_name, char* server_name, uint16_t port);
			void stop_mdns(char* host_name, char* server_name, uint16_t port);

			/* Open a client connection. */
			ESP8266Socket* open_tcp(char* address, uint16_t port);
			ESP8266Socket* open_ucp(char* address, uint16_t port);

			/* Read incoming data and selects the channel the data belongs to.
			 * Output buffer is flushed and input buffer will contain the data to read.
			 * Returns the id of the connection that was selected.
			 * A server should call this repeatedly to receive new requests.
			 * A client should call this after flush when response is expected.
			 * Returns the number of bytes available to read.
			 */
			ESP8266Socket* accept();

			friend class ESP8266Socket;
	};
}

#endif
