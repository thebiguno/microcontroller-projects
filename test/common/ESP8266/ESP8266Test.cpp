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
	mockStream.clear();
	test_at_rst(); mockStream.clear();
	test_at__echo(); mockStream.clear();
	test_at__noecho(); mockStream.clear();
	test_at_cwmode_cur_sta(); mockStream.clear();
	test_at_cwjap_cur__ok(); mockStream.clear();
	test_at_cwjap_cur__fail(); mockStream.clear();
	test_at_cwqap(); mockStream.clear();
	test_at_cifsr(); mockStream.clear();
	test_at_cipmux(); mockStream.clear();
	test_at_cipserver(); mockStream.clear();
	test_at_cipstart_tcp();	mockStream.clear();
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
	assert("at_cwjap_cur didn't fail", ok);
}

void ESP8266Test::test_at_cwqap() {
	mockStream.enqueue("AT+CWQAP\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cwqap();
	assert("at_cwqap failed", ok);
}

void ESP8266Test::test_at_cifsr() {
	// 192.168.0.4 = C0:A8:00:04 = 3_232_235_524
	mockStream.enqueue("AT+CIFSR\r\n\r\n+CIFSR:STAIP,\"192.168.0.4\"\r\nCIFSR:STAMAC,\"xx:xx:xx:xx:xx:xx\"\r\n\r\nOK\r\n");
	uint32_t address = wifi.at_cifsr();
	assert("incorrect address returned by at_cifsr", 3232235524 == address);
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
}

void ESP8266Test::test_at_cipstart_tcp() {
	char buf[32];

	mockStream.enqueue("AT+CIPSTART=\"TCP\",\"192.168.0.4\",8080\r\n\r\n\r\nOK\r\n");
	ESP8266Socket* s = wifi.at_cipstart_tcp("192.168.0.4", 8080);
	assert("at_cipstart_tcp didn't return a socket", s != NULL);

	mockStream.clear();

	mockStream.enqueue("AT+CIPSEND=0,4\r\n> SEND OK\r\n");
	s->write("test");
	s->flush();

	mockStream.dequeue(buf, 21);
	assert("flushing socket didn't result in at_cipsend", 0 == strcmp("AT+CIPSEND=0,4\r\ntest", buf));
}

void ESP8266Test::assert(const char* message, char* expected, char* actual) {
	if (strcmp(expected, actual) != 0) {
		puts(message);
		puts("\n");
	}
}

void ESP8266Test::assert(const char* message, uint8_t expected, uint8_t actual) {
		if (expected != actual) {
			puts(message);
			puts("\n");
		}
}

void ESP8266Test::assert(const char* message, uint8_t truth) {
		if (!truth) {
			puts(message);
			puts("\n");
		}
}
