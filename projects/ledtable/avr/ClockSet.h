#ifndef ClockSet_H
#define ClockSet_H

#include "Module.h"
#include <stdint.h>
#include "lib/mcp79410.h"

namespace digitalcave {
	class ClockSet : public Module {
	public:
		ClockSet();
		~ClockSet();
		void run();
	};
}

#endif