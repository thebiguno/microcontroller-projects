#ifndef MOCK_STREAM_H
#define MOCK_STREAM_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	/* Used to simulate intaction with the serial port */
	class MockStream : public Stream {

		private:
			ArrayStream input;
			ArrayStream output;
		public:
			MockStream(uint16_t input_buffer_length, uint16_t output_buffer_length);
			~MockStream();

			/* Make data available to read */
			uint8_t enqueue(const char* a);
			/* Get data that was written */
			uint16_t dequeue(char* a, uint16_t len);

			void clear();

			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);
			uint8_t flush();

			using Stream::reset;
			using Stream::write;
	};
}

#endif
