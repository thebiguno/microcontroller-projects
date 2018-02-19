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
	test_at_rst();
	test_at__echo();
	test_at__noecho();
	test_at_cwmode_cur_sta();
	test_at_cwjap_cur__ok();
	test_at_cwjap_cur__fail();
	test_at_cwqap();
	test_at_cifsr();
	test_at_cipmux();
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
	char ip[16];
	char mac[18];
	// 192.168.0.4 = C0:A8:00:04 = 3_232_235_524
	mockStream.enqueue("AT+CIFSR\r\n\r\n+CIFSR:STAIP,\"yyy.yyy.yyy.yyy\"\r\n+CIFSR:STAMAC,\"xx:xx:xx:xx:xx:xx\"\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cifsr(ip, mac);
	assert("at_cifsr failed", ok);
	assert("incorrect ip returned by at_cifsr", (char*)"yyy.yyy.yyy.yyy", ip);
	assert("incorrect mac address returned by at_cifsr", (char*)"xx:xx:xx:xx:xx:xx", mac);
}

void ESP8266Test::test_at_cipmux() {
	mockStream.enqueue("AT+CIPMUX=1\r\n\r\n\r\nOK\r\n");
	uint8_t ok = wifi.at_cipmux(1);
	assert("at_cipmux failed", ok);
}

void ESP8266Test::test_at_cipserver() {
	mockStream.enqueue("\r\n\r\nOK\r\n");
	puts("* at_cipserver");
	assert("at_cipserver didn't return ok", wifi.at_cipserver(1, 8080));

	mockStream.enqueue("+IPD,4,13:Hello World\r\n");
	puts("* accept");
	ESP8266Stream* s = wifi.accept();
	assert("accept didn't return a stream", s != NULL);
	assert("stream has wrong id", 4, s->id());

	mockStream.enqueue("AT+CIPCLOSE=4\r\n\r\n4,CLOSED\r\n\r\nOK\r\n");
	puts("* close");
	assert("close didn't return ok", 1, s->close());
}

void ESP8266Test::test_at_cipstart_tcp() {
	char buf[32];

	mockStream.enqueue("*echo*\r\n\r\n\r\nOK\r\n");
	puts("* at_cipstart_tcp");
	ESP8266Stream* s = wifi.at_cipstart_tcp("192.168.0.4", 8080);
	assert("at_cipstart_tcp didn't return a stream", s != NULL);

	mockStream.enqueue("AT+CIPSEND=0,4\r\n> ");
	puts("* write");
	assert("write didn't write 4 chars", 4, s->write("test"));
	mockStream.enqueue("\r\nRecv 4 bytes\r\nSEND OK\r\n");
	puts("* flush");
	assert("flush didn't return ok", 1, s->flush());

	mockStream.dequeue(buf, 20);
	assert("flush didn't result in cipsend", "AT+CIPSEND=0,4\r\ntest", buf);

	mockStream.enqueue("*echo*\r\n\r\n4,CLOSED\r\n\r\nOK\r\n");
	puts("* close");
	assert("close didn't return ok", 1, s->close());

	mockStream.dequeue(buf, 15);
	assert("close didn't result in cipclose", "AT+CIPCLOSE=0\r\n", buf);
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
