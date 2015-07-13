#ifndef Mood_H
#define Mood_H

#include "Module.h"
#include "Matrix.h"
#include <stdint.h>

namespace digitalcave {
	class Mood : public Module {
	public:
		Mood();
		~Mood();
		void run();
	};
}

#endif