#include "JsonParser.h"

#include <string.h>
#include <stdio.h>
#include <dcstring.h>

#define TOKEN_UNKNOWN 255

using namespace digitalcave;

JsonParser::JsonParser(Stream* stream, uint8_t buffer_size) :
    stream(stream),
    buffer(ArrayStream(buffer_size)),
    token(TOKEN_UNKNOWN),
    next(0xff)
{}

JsonParser::~JsonParser() {
}

uint8_t JsonParser::currentToken() {
    return token;
}

uint8_t JsonParser::nextToken() {
    if (next < TOKEN_UNKNOWN) {
        uint8_t result = next;
        next = TOKEN_UNKNOWN;
        return result;
    }

    uint8_t b;
    buffer.clear();
    token = TOKEN_UNKNOWN;
    while (stream->read(&b)) {
        if (token == TOKEN_STRING) {
            if (b == '"') {
                if (stream->read(&b)) {
                    if (b == ':') {
                        token = TOKEN_KEY;
                        return token;
                    } else if (b == ',') {
                        return token;
                    } else if (b == ']') {
                        next = TOKEN_END_ARRAY;
                        return token;
                    } else if (b == '}') {
                        next = TOKEN_END_OBJECT;
                        return token;
                    } else {
                        // not possible
                        return token;
                    }
                }
            } else if (b == '\\') {
                if (stream->read(&b)) {
                    if (b == 'u') {
                        //uint16_t codepoint = 0;
                        for (uint8_t i = 0; i < 4; i++) {
                            stream->read(&b);
                            /*
                            if (b >= '0' && b <= '9') {
                                codepoint &= (b - '0')
                            } else if (b >= 'a' && b <= 'f') {
                                codepoint &= (b - 'a' + 10);
                            } else if (b >= 'A' && b <= 'F') {
                                codepoint &= (b - 'A' + 10);
                            }
                            codepoint << 0x0f;
                            */
                        }
                        buffer.write('?');
                    } else {
                        b = b == '"' ? '"'
                          : b == '/' ? '/'
                          : b == '\\' ? '\\'
                          : b == 'b' ? '\b'
                          : b == 'f' ? '\f'
                          : b == 'n' ? '\n'
                          : b == 'r' ? '\r'
                          : b == 't' ? '\t'
                          : b;
                        buffer.write(b);
                    }
                }
            } else {
                buffer.write(b);
            }
        } else if (b == 't') {
            token = TOKEN_TRUE;
            buffer.write(b);
        } else if (b == 'f') {
            token = TOKEN_FALSE;
            buffer.write(b);
        } else if (b == 'n') {
            token = TOKEN_NULL;
            buffer.write(b);
        } else if (b == '{') {
            token = TOKEN_START_OBJECT;
            return token;
        } else if (b == '[') {
            token = TOKEN_START_ARRAY;
            return token;
        } else if (b == '}') {
            if (!buffer.isEmpty()) {
                next = TOKEN_END_OBJECT;
            } else {
                token = TOKEN_END_OBJECT;
            }
            return token;
        } else if (b == ']') {
            if (!buffer.isEmpty()) {
                next = TOKEN_END_ARRAY;
            } else {
                token = TOKEN_END_ARRAY;
            }
            return token;
        } else if (b == ',') {
            return token;
        } else if (b == ' ' || b == '\t' || b == '\n' || b == '\r') {
            if (!buffer.isEmpty()) {
                return token;
            }
        } else if (b == '-' || (b >= '0' && b <= '9')) {
            buffer.write(b);
            token = TOKEN_NUMBER;
        } else if (b == '"') {
            token = TOKEN_STRING;
        } else {
            buffer.write(b);
        }
    }

    if (!buffer.isEmpty()) {
        next = TOKEN_EOF;
        return token;
    } else {
        return TOKEN_EOF;
    }

}

void JsonParser::getValue(char* buf) {
    uint8_t i = 0;
    uint8_t b;
    while (buffer.read(&b)) {
        buf[i++] = (char) b;
    }
    buf[i] = 0;
}