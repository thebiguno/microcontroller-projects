#ifndef CALIBRATEPAD_H
#define CALIBRATEPAD_H

#include <Wire.h>
#include <EEPROM/EEPROM.h>

#include "../hardware.h"
#include "Menu.h"

namespace digitalcave {

	class CalibratePad : public Menu {
	
		private:
			static uint8_t getAddress(uint8_t channel);
			static uint8_t getMemoryAddress(uint8_t channel);
			
			static uint16_t readValue(uint8_t channel);
			static void writeValue(uint8_t channel, uint16_t value);
			
		public:
			static void loadFromEeprom();
			static void saveToEeprom();
		
			int16_t value = -1;
			uint8_t channel = 0;
			
			CalibratePad();
			Menu* handleAction();
	};
}

#endif