#ifndef Plasma_H
#define Plasma_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
	class Plasma : public Module {
	public:
		Plasma();
		~Plasma();
		void run();
	};
}

#endif