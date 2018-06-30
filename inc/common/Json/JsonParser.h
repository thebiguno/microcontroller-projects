#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#define TOKEN_EOF 0
#define TOKEN_START_OBJECT 1
#define TOKEN_END_OBJECT 2
#define TOKEN_START_ARRAY 3
#define TOKEN_END_ARRAY 4
#define TOKEN_NULL 5
#define TOKEN_TRUE 6
#define TOKEN_FALSE 7
#define TOKEN_NUMBER 8
#define TOKEN_STRING 9
#define TOKEN_KEY 10

#include "Stream.h"
#include "ArrayStream.h"

namespace digitalcave {
    class JsonParser {

        private:
            Stream* stream;
            ArrayStream buffer;
            uint8_t token;
            uint8_t next;
        public:
            JsonParser(Stream* stream, uint8_t buffer_size);
            ~JsonParser();

            uint8_t nextToken();
            uint8_t currentToken();
            void getValue(char* buffer);
    };
}

#endif
