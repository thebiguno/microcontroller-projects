#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include "ESP8266.h"

#include "WebServerConn.h"

namespace digitalcave {
	class WebServer {

		private:
			ESP8266* wifi;
			WebServerConn* conns[5];
		public:
			/* Consume the entire ESP8266 module for the task of being a webserver. */
			WebServer(ESP8266* wifi);
			~WebServer();

			/* Reads incoming request headers */
			WebServerConn* accept();

			friend class WebServerConn;
	};
}

#endif
