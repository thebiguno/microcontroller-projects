#ifndef AltClock_H
#define AltClock_H

#include <Module.h>
#include <stdint.h>
#include "matrix.h"
#include "lib/rtc/mcp79410.h"

namespace digitalcave {
	class AltClock : public Module {
	public:
		AltClock();
		~AltClock();
	}
}

#endif