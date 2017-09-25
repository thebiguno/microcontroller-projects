#ifndef ESP8266_H
#define ESP8266_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	/* A stream wrapper for an ESP8266 Wifi module.
	 * Requests and responses are fully buffered with a shared buffer.
	 * Provides single threaded access to channels which means you can't act
	 * as a client and server at the same time, or act as multiple servers
	 * at the same time.
	 */
	class ESP8266 : public Stream {

		private:
			Stream* serial;
			ArrayStream buffer;

			uint32_t addr;
			uint16_t length;
			uint16_t position;

			uint8_t id;							// the current connection id (0 - 4);

			void poll();						// process whatever is in the serial receive buffer
			void at_reset();				// send AT+RST
			void at_mode();					// send AT+CWMODE=1 to configure station mode
			void at_cifsr();				// send AT+CIFSR to lease an IP address
			void at_mux();					// send AT+CIPMUX=1 to configure multiple connection
			uint8_t at_response(char* status);	// handle the responses for AT commands

		public:
			ESP8266(Stream* serial, uint16_t rxtxBufferSize = 1024);
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

			/* Open a client connection. */
			void open_tcp(char* address, uint16_t port);
			void open_ucp(char* address, uint16_t port);

			/* Close the current connection */
			uint8_t close();

			/* Read incoming data and selects the channel the data belongs to.
			 * Output buffer is flushed and input buffer will contain the data to read.
			 * Returns the id of the connection that was selected.
			 * A server should call this repeatedly to receive new requests.
			 * A client should call this after flush when response is expected.
			 * Returns the number of bytes available to read.
			 */
			uint16_t accept();

			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);

			/* Clears the buffer */
			void clear();

			/* Flushes the buffer */
			uint8_t flush();

			using Stream::reset;
			using Stream::write;
	};
}

#endif
