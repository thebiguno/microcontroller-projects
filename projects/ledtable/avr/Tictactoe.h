#ifndef Tictactoe_H
#define Tictactoe_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
	class Tictactoe : public Module {
	public:
		Tictactoe();
		~Tictactoe();
		void run();
	};
}

#endif