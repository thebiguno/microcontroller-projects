#ifndef TestPattern_H
#define TestPattern_H

#include <Module.h>
#include <stdint.h>
#include "Color.h"
#include "matrix.h"
#include "lib/psx/psx.h"

#define LIFE_HASH_COUNT			20
#define LIFE_MATCH_COUNT		20

namespace digitalcave {
	class TestPattern : public Module {
	
	public:
		TestPattern();
		~TestPattern();
		void run();
	};
}

#endif