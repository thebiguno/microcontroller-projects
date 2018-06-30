#include "JsonParserTest.h"

#include <string.h>

#include "JsonParser.h"

using namespace digitalcave;

JsonParserTest::JsonParserTest() {}

JsonParserTest::~JsonParserTest() {}

void JsonParserTest::test() {
    testObject();
    testArray();
    testStringEscape();
}

void JsonParserTest::testObject() {
    puts("* object");
    char buf[10];

    MockStream mockStream = MockStream(2048, 2048);
    mockStream.enqueue("{\"string\": \"hello\", \"boolean\": true, \"number\": 0.123e+3, \"null\": null}");

    JsonParser parser = JsonParser(&mockStream, 10);
    uint8_t token;

    token = parser.nextToken();
    assert("invalid start object token", TOKEN_START_OBJECT, token);

    token = parser.nextToken();
    assert("invalid key token (string)", TOKEN_KEY, token);
    parser.getValue(buf);
    assert("invalid key value (string)", "string", buf);

    token = parser.nextToken();
    assert("invalid string token", TOKEN_STRING, token);
    parser.getValue(buf);
    assert("invalid string value", "hello", buf);

    token = parser.nextToken();
    assert("invalid key token (boolean)", TOKEN_KEY, token);
    parser.getValue(buf);
    assert("invalid key value (boolean)", "boolean", buf);

    token = parser.nextToken();
    assert("invalid true token", TOKEN_TRUE, token);
    parser.getValue(buf);
    assert("invalid true value", "true", buf);

    token = parser.nextToken();
    assert("invalid key token (number)", TOKEN_KEY, token);
    parser.getValue(buf);
    assert("invalid key value (number)", "number", buf);

    token = parser.nextToken();
    assert("invalid number token", TOKEN_NUMBER, token);
    parser.getValue(buf);
    assert("invalid number value", "0.123e+3", buf);

    token = parser.nextToken();
    assert("invalid key token (null)", TOKEN_KEY, token);
    parser.getValue(buf);
    assert("invalid key value (null)", "null", buf);

    token = parser.nextToken();
    assert("invalid null token", TOKEN_NULL, token);
    parser.getValue(buf);
    assert("invalid null value", "null", buf);

    token = parser.nextToken();
    assert("invalid end object token", TOKEN_END_OBJECT, token);

    token = parser.nextToken();
    assert("invalid eof token", TOKEN_EOF, token);
}

void JsonParserTest::testArray() {
    puts("* array");
    char buf[10];

    MockStream mockStream = MockStream(2048, 2048);
    mockStream.enqueue("[\"hello\", true, 0.123e+3, null]");

    JsonParser parser = JsonParser(&mockStream, 10);
    uint8_t token;

    token = parser.nextToken();
    assert("invalid start array token", TOKEN_START_ARRAY, token);

    token = parser.nextToken();
    assert("invalid string token", TOKEN_STRING, token);
    parser.getValue(buf);
    assert("invalid string value", "hello", buf);

    token = parser.nextToken();
    assert("invalid true token", TOKEN_TRUE, token);
    parser.getValue(buf);
    assert("invalid true value", "true", buf);

    token = parser.nextToken();
    assert("invalid number token", TOKEN_NUMBER, token);
    parser.getValue(buf);
    assert("invalid number value", "0.123e+3", buf);

    token = parser.nextToken();
    assert("invalid null token", TOKEN_NULL, token);
    parser.getValue(buf);
    assert("invalid null value", "null", buf);

    token = parser.nextToken();
    assert("invalid end array token", TOKEN_END_ARRAY, token);

    token = parser.nextToken();
    assert("invalid eof token", TOKEN_EOF, token);
}

void JsonParserTest::testStringEscape() {
    puts("* string escape");
    char buf[255];

    MockStream mockStream = MockStream(2048, 2048);
    mockStream.enqueue("\"quote \\\" slash \\/ backslash \\\\ backspace \\\b formfeed \\\f newline \\\n return \\\r tab \\\t unicode \\uabcd\"");

    JsonParser parser = JsonParser(&mockStream, 255);
    uint8_t token;

    token = parser.nextToken();
    assert("invalid string token", TOKEN_STRING, token);
    parser.getValue(buf);
    assert("invalid string value", "quote \" slash / backslash \\ backspace \b formfeed \f newline \n return \r tab \t unicode ?", buf);

    token = parser.nextToken();
    assert("invalid eof token", TOKEN_EOF, token);
}

