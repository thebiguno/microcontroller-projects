#ifndef STATS_H
#define STATS_H

#include "Menu.h"

namespace digitalcave {

	class Stats : public Menu {
	
		private:
			uint32_t lastUpdate;
			uint8_t forceUpdate;
			
		public:
			Stats();
			Menu* handleAction();
	};
}

#endif