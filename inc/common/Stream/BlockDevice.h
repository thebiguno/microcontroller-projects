#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include "Stream.h"

namespace digitalcave {
	class BlockDevice : Stream {

	public:
		virtual void setBlock(uint32_t address) = 0;

		using Stream::reset;
		using Stream::skip;
		using Stream::read;
		using Stream::write;
	};
}

#endif
