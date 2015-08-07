#ifndef LOADSAMPLES_H
#define LOADSAMPLES_H

#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "Menu.h"

namespace digitalcave {

	class LoadSamples : public Menu {
	
		private:
			std::vector<String> folders;
			void ensureFoldersLoaded();
			
		public:
			LoadSamples();
			Menu* handleAction();
	};
}

#endif