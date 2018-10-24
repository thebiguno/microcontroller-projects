#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

namespace digitalcave {

	class Test {

		protected:
			void assert(const char* message, uint8_t truth);
			void assert(const char* message, const char* expected, char* actual);
			void assert(const char* message, uint32_t expected, uint32_t actual);
			void assert_64(const char* message, int64_t expected, int64_t actual);

		public:
			Test();
			~Test();

			void test();
	};
}

#endif
