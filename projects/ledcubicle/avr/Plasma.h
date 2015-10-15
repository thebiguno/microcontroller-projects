#ifndef Plasma_H
#define Plasma_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
	class Plasma : public Module {
	private:
		uint8_t baseColor;
		
	public:
		Plasma(uint8_t baseColor);
		~Plasma();
		void run();
	};
}

#endif