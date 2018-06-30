#include "Test.h"

#include <string.h>

using namespace digitalcave;

Test::Test() {}

Test::~Test() {}

void Test::assert(const char* message, const char* expected, char* actual) {
    if (strcmp(expected, actual) != 0) {
        puts(message);
        abort();
    }
}

void Test::assert(const char* message, uint32_t expected, uint32_t actual) {
    if (expected != actual) {
        puts(message);
        abort();
    }
}

void Test::assert(const char* message, uint8_t truth) {
    if (!truth) {
        puts(message);
        abort();
    }
}

