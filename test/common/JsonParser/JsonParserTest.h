#ifndef JSON_PARSER_TEST_H
#define JSON_PARSER_TEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "Test.h"
#include "MockStream.h"

namespace digitalcave {

    class JsonParserTest : Test {

        private:
            void testObject();
            void testArray();
            void testStringEscape();

        public:
            JsonParserTest();
            ~JsonParserTest();

            void test();
    };
}

#endif
