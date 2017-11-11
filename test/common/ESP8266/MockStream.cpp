#include "MockStream.h"

using namespace digitalcave;

MockStream::MockStream(uint16_t input_buffer_length, uint16_t output_buffer_length) :
	input(ArrayStream(input_buffer_length)),
	output(ArrayStream(output_buffer_length))
{}

MockStream::~MockStream() {
}

void MockStream::clear() {
	output.clear();
}

uint8_t MockStream::read(uint8_t* b) {
	return input.read(b);
}

uint8_t MockStream::write(uint8_t b) {
	return output.write(b);
}

uint8_t MockStream::enqueue(const char* a) {
	output.clear();
	return input.write(a);
}
uint16_t MockStream::dequeue(uint8_t* a, uint16_t len) {
	return output.read((uint8_t*)a, len);
}
