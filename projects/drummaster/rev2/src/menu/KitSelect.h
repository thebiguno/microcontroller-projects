#ifndef KITSELECT_H
#define KITSELECT_H

#include "../util/Mapping.h"
#include "Menu.h"
#include "../hardware.h"

namespace digitalcave {

	class KitSelect : public Menu {
	
		private:
			int8_t lastKitIndex;
			Mapping mapping;
			
		public:
			KitSelect();
			Menu* handleAction();
	};
}

#endif