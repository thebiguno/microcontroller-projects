#include "ESP8266Test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace digitalcave;

ESP8266Test::ESP8266Test() :
	mockStream(2048, 2048),
	wifi(&mockStream)
{}

ESP8266Test::~ESP8266Test() {
}

void ESP8266Test::test() {
	// test_at_rst();
	// test_at__echo();
	// test_at__noecho();
	// test_at_cwmode_cur_sta();
	// test_at_cwjap_cur__ok();
	// test_at_cwjap_cur__fail();
	// test_at_cwqap();
	// test_at_cifsr();
	// test_at_cipmux();
	test_at_cipserver();
	test_at_cipstart_tcp();
}

void ESP8266Test::test_at_rst() {
	mockStream.enqueue("AT+RST\r\n\r\n\r\nOK\r\n");
	mockStream.enqueue("a bunch of\tgarbage");
	uint8_t ok = wifi.at_rst();
	assert("at_rst failed", ok);
}

void ESP8266Test::test_at__echo() {
	mockStream.enqueue("AT\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at();
	assert("at with echo failed", ok);
}

void ESP8266Test::test_at__noecho() {
	mockStream.enqueue("\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at();
	assert("at without echo failed", ok);
}

void ESP8266Test::test_at_cwmode_cur_sta() {
	mockStream.enqueue("AT+CWMODE_CUR=1\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cwmode_cur_sta();
	assert("at_cwmode_cur_sta failed", ok);
}

void ESP8266Test::test_at_cwjap_cur__ok() {
	mockStream.enqueue("AT+CWJAP_CUR=\"test\",\"pass\"\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cwjap_cur((char*)"test",(char*)"pass");
	assert("at_cwjap_cur failed", ok);
}

void ESP8266Test::test_at_cwjap_cur__fail() {
	mockStream.enqueue("AT+CWJAP_CUR=\"test\",\"pass\"\r\n\r\n+CWJAP:1\r\n\r\nFAIL\r\n");
	uint8_t ok = wifi.at_cwjap_cur((char*)"test",(char*)"pass");
	assert("at_cwjap_cur didn't fail", !ok);
}

void ESP8266Test::test_at_cwqap() {
	mockStream.enqueue("AT+CWQAP\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cwqap();
	assert("at_cwqap failed", ok);
}

void ESP8266Test::test_at_cifsr() {
	char addr[16];
	// 192.168.0.4 = C0:A8:00:04 = 3_232_235_524
	mockStream.enqueue("AT+CIFSR\r\n\r\n+CIFSR:STAIP,\"192.168.0.4\"\r\nCIFSR:STAMAC,\"xx:xx:xx:xx:xx:xx\"\r\n\r\nOK\r\n");
	uint32_t decimal = wifi.at_cifsr(addr);
	puts(addr);
	assert("incorrect string address returned by at_cifsr", (char*)"192.168.0.4", addr);
	assert("incorrect decimal address returned by at_cifsr", 3232235524, decimal);
}

void ESP8266Test::test_at_cipmux() {
	mockStream.enqueue("AT+CIPMUX=1\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cipmux(1);
	assert("at_cipmux failed", ok);
}

void ESP8266Test::test_at_cipserver() {
	mockStream.enqueue("\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cipserver(1, 8080);
	assert("at_cipserver failed", ok);

	mockStream.enqueue("+IPD,4,13:Hello World\r\n");
	ESP8266Socket* s = wifi.accept();
	assert("accept didn't return a socket", s != NULL);
	assert("socket has wrong id", 4, s->id());

	mockStream.enqueue("AT+CIPCLOSE=0\r\n\r\n4,CLOSED\r\n\r\nOK\r\n");
	s->close();
}

void ESP8266Test::test_at_cipstart_tcp() {
	uint8_t buf[32];

	mockStream.enqueue("AT+CIPSTART=\"TCP\",\"192.168.0.4\",8080\r\n\r\n\r\nOK\r\n");
	ESP8266Socket* s = wifi.at_cipstart_tcp("192.168.0.4", 8080);
	assert("at_cipstart_tcp didn't return a socket", s != NULL);

	mockStream.enqueue("AT+CIPSEND=0,4\r\n> ");
	s->write("test");
	mockStream.enqueue("\r\nRecv 4 bytes\r\nSEND OK\r\n");
	puts("flush");
	s->flush();

	mockStream.dequeue(buf, 21);
	buf[20] = 0;
	assert("flushing socket didn't result in at_cipsend", "AT+CIPSEND=0,4\r\ntest", (char*)buf);

	mockStream.enqueue("AT+CIPCLOSE=0\r\n\r\n4,CLOSED\r\n\r\nOK\r\n");
	s->close();
}

void ESP8266Test::assert(const char* message, const char* expected, char* actual) {
	if (strcmp(expected, actual) != 0) {
		puts(message);
		abort();
	}
}

void ESP8266Test::assert(const char* message, uint32_t expected, uint32_t actual) {
	if (expected != actual) {
		puts(message);
		abort();
	}
}

void ESP8266Test::assert(const char* message, uint8_t truth) {
	if (!truth) {
		puts(message);
		abort();
	}
}
