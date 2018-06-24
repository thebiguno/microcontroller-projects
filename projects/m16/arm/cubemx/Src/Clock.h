#ifndef Clock_H
#define Clock_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
	class Clock : public Module {
	public:
		Clock();
		~Clock();
		void run();
	};
}

#endif