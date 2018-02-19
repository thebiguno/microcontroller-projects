#ifndef WEB_TEST_H
#define WEB_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <WebStream.h>

#include "MockStream.h"

namespace digitalcave {

	class WebTest {

		private:
			void assert(const char* message, const char* expected, char* actual);
			void assert(const char* message, uint32_t expected, uint32_t actual);

			void test_client_get();
			void test_client_put_identity();
			void test_client_put_chunked();
			void test_server_get();
		public:
			WebTest();
			~WebTest();

			void test();
	};
}

#endif
