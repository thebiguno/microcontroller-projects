#include "Test.h"

#include <string.h>

using namespace digitalcave;

Test::Test() {}

Test::~Test() {}

void Test::assert(const char* message, const char* expected, char* actual) {
    if (strcmp(expected, actual) != 0) {
       printf("%s: expected %s, got %s\n", message, expected, actual);
        abort();
    }
}

void Test::assert(const char* message, uint32_t expected, uint32_t actual) {
    if (expected != actual) {
        printf("%s: expected %u, got %u\n", message, expected, actual);
        abort();
    }
}

void Test::assert_64(const char* message, int64_t expected, int64_t actual) {
    if (expected != actual) {
        printf("%s: expected %lli, got %lli\n", message, expected, actual);
        abort();
    }
}

void Test::assert(const char* message, uint8_t truth) {
    if (!truth) {
        puts(message);
        abort();
    }
}

