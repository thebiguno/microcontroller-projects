#include "WebTest.h"

#include <string.h>

using namespace digitalcave;

WebTest::WebTest() {}

WebTest::~WebTest() {}

void WebTest::test() {
	test_client_get();
	test_client_put_identity();
	test_client_put_chunked();
	test_server_get();
}

void WebTest::test_client_get() {
	puts("* client get");
	char buf[64];

	MockStream mockStream = MockStream(2048, 2048);
	WebStream stream = WebStream(&mockStream);
	stream.request("GET", "/hello?world", NULL);
	stream.send_header("Accept", "x");
	stream.send_identity("");

	mockStream.dequeue(buf, 40);
	puts(buf);
	assert("incorrect request", "GET /hello?world HTTP/1.1\r\nAccept: x\r\n\r\n", buf);

	mockStream.enqueue("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello");
	assert("incorrect content length", 5, stream.content_length());
	assert("incorrect status", 200, stream.status());

	uint16_t read = stream.read((uint8_t*)buf, stream.content_length());
	buf[read] = 0;
	puts(buf);
	assert("incorrect entity", "Hello", buf);
}

void WebTest::test_client_put_identity() {
	puts("* client put");
	char buf[128];

	MockStream mockStream = MockStream(2048, 2048);
	WebStream stream = WebStream(&mockStream);
	stream.request("PUT", "/hello?world", "text/plain");
	stream.send_identity("Hello");

	mockStream.dequeue(buf, 79);
	puts(buf);
	assert("incorrect request", "PUT /hello?world HTTP/1.1\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nHello", buf);

	mockStream.enqueue("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello");
	assert("incorrect status", 200, stream.status());
	assert("incorrect content length", 5, stream.content_length());

	uint16_t read = stream.read((uint8_t*) buf, stream.content_length());
	buf[read] = 0;
	assert("incorrect entity", "Hello", buf);
}

void WebTest::test_client_put_chunked() {
	puts("* client put chunked");
	char buf[128];

	MockStream mockStream = MockStream(2048, 2048);
	WebStream stream = WebStream(&mockStream);
	stream.request("PUT", "/hello", "text/plain");
	stream.start_chunked();
	stream.send_chunk("Hello");
	stream.send_chunk("World");
	stream.send_chunk("");

	mockStream.dequeue(buf, 102);
	puts(buf);
	assert("incorrect request", "PUT /hello HTTP/1.1\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n5\r\nWorld\r\n0\r\n\r\n", buf);

	mockStream.enqueue("HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n5\r\nWorld\r\n0\r\n\r\n");
	assert("incorrect status", 200, stream.status());
	assert("incorrect content length", 0, stream.content_length());

	uint16_t read = stream.read((uint8_t*) buf, 10);
	buf[read] = 0;
	puts(buf);
	assert("incorrect entity", "HelloWorld", buf);
}

void WebTest::test_server_get() {
	puts("* server get");
	char buf[128];

	MockStream mockStream = MockStream(2048, 2048);
	WebStream stream = WebStream(&mockStream);
	mockStream.enqueue("GET /hello?world HTTP/1.1\r\n\r\n");
	// mockStream.read((uint8_t*)buf, 29);
	buf[29] = 0;
	puts(buf);
	puts(stream.method());
	assert("incorrect method", "GET", stream.method());
	assert("incorrect path", "/hello", stream.path());
	assert("incorrect query", "world", stream.query());
	assert("incorrect content length", 0, stream.content_length());

	stream.response(200, "text/plain");
	stream.send_identity("Hello");

	mockStream.dequeue(buf, 69);
	puts(buf);
	assert("incorrect request", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nHello", buf);
}

void WebTest::assert(const char* message, const char* expected, char* actual) {
	if (strcmp(expected, actual) != 0) {
		puts(message);
		abort();
	}
}

void WebTest::assert(const char* message, uint32_t expected, uint32_t actual) {
	if (expected != actual) {
		puts(message);
		abort();
	}
}
