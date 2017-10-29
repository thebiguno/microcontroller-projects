#include "WebServer.h"

#include <string.h>
#include <dcstring.h>

using namespace digitalcave;

WebServer::WebServer(ESP8266* wifi) :
	wifi(wifi)
{
	wifi->start_server(80);
}

WebServer::~WebServer() {
}

WebServerConn* WebServer::accept() {
	ESP8266Socket* socket = wifi->accept();
	if (socket == NULL) return NULL;

	uint8_t id = socket->id();
	if (conns[id] == NULL) {
		conns[id] = new WebServerConn(this, socket);
		conns[id]->read_headers();
	}
	return conns[id];
}
