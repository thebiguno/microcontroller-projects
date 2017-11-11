#ifndef ESP8266_TEST_H
#define ESP8266_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <ESP8266.h>
#include <ESP8266Socket.h>
#include "MockStream.h"

namespace digitalcave {

	/* Used to simulate intaction with the serial port */
	class ESP8266Test {

		private:
			MockStream mockStream;
			ESP8266 wifi;

			void assert(const char* message, uint8_t truth);
			void assert(const char* message, const char* expected, char* actual);
			void assert(const char* message, uint32_t expected, uint32_t actual);

			void test_at_rst();
			void test_at__echo();
			void test_at__noecho();
			void test_at_cwmode_cur_sta();
			void test_at_cwjap_cur__ok();
			void test_at_cwjap_cur__fail();
			void test_at_cwqap();
			void test_at_cifsr();
			void test_at_cipmux();
			void test_at_cipserver();
			void test_at_cipstart_tcp();
		public:
			ESP8266Test();
			~ESP8266Test();

			void test();
	};
}

#endif
