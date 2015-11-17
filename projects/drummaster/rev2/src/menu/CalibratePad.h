#ifndef CALIBRATEPAD_H
#define CALIBRATEPAD_H

#include <Wire.h>

#include "../hardware.h"
#include "Menu.h"

namespace digitalcave {

	class CalibratePad : public Menu {
	
		private:
			uint16_t readValue(uint8_t address, uint8_t memoryAddress);
			void writeValue(uint8_t address, uint8_t memoryAddress, uint16_t value);
			
		public:
			int16_t value = -1;
			uint8_t selectedPad = 0;
			
			CalibratePad();
			Menu* handleAction();
	};
}

#endif