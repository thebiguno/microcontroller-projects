#ifndef ESP8266_H
#define ESP8266_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {
	class ESP8266 : public Stream {

		private:
			Stream* serial;
			ArrayStream txBuffer;

			uint16_t length;
			uint16_t position;
			uint8_t* data;

			int8_t id;							// the current connection id (0 - 4)

			void poll();						// process whatever is in the serial receive buffer
			void at_reset();				// send AT+RST
			void at_mode();					// send AT+CWMODE=1 to configure station mode
			void at_mux();					// send AT+CIPMUX=1 to configure multiple connection
			uint8_t at_response();	// handle the responses for atCommands

		public:
			ESP8266(Stream* serial, uint8_t txBufferSize);
			~ESP8266();

			uint8_t join(char* ssid, char* password);
			void leave();

			/* Start a server connection listener. */
			void start_server(uint16_t port);
			/* Stop a server connection listener.  */
			void stop_server(uint16_t port);

			/* Open a client connection.
			 * Returns the ID of the connection or -1 if the connection couldn't be opened. */
			int8_t open(char* address, uint16_t port);
			/* Close a client connection.
			 * Returns 1 if the connection was closed, or 0 if it wasn't open */
			uint8_t close(uint8_t i);

			/* Select a connection to write to.
			 * Output buffer is flushed and input buffer is cleared. */
			void select(uint8_t i);

			/* Read incoming data and selects the channel the data belongs to.
			 * Output buffer is flushed and input buffer will contain the data to read.
			 * Returns the id of the connection that was selected. */
			uint8_t select();

			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);

			/* Flushes the output buffer.
			 * Returns 1 if the data is successfuly transmitted. */
			uint8_t flush();

			using Stream::reset;
			using Stream::write;
	};
}

#endif
