#include "VolumePad.h"

using namespace digitalcave;

VolumePad::VolumePad(){
}

Menu* VolumePad::handleAction(){
	if (value == -1){
		value = Pad::pads[pad]->getVolume();
		encoder.write(value);
	}
	
	encoderState = encoder.read();
	if (encoderState != value){
		Serial.println("Diff!");
		if (encoderState > 255){
			Serial.println("Overflow!");
			encoderState = 255;
			encoder.write(255);
		}
		else if (encoderState < 0){
			Serial.println("Underflow!");
			encoderState = 0;
			encoder.write(0);
		}
		
		value = encoderState;
		Pad::pads[pad]->setVolume(value);
	}

	snprintf(buf, sizeof(buf), "%d%%      ", (uint16_t) (value / 64.0 * 100));
	display.write_text(2, 0, buf, 5);

	if (button.fallingEdge()){
		EEPROM.update(EEPROM_PAD_VOLUME + pad, (uint8_t) value);
		display.write_text(2, 0, "                    ", 20);
		return Menu::volumePadSelect;
	}
	
	return NULL;
}

void VolumePad::loadPadVolumesFromEeprom(){
	uint16_t values[PAD_COUNT];
	EEPROM.get(EEPROM_PAD_VOLUME, values);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		Pad::pads[i]->setVolume(values[i]);
	}
}

void VolumePad::savePadVolumesToEeprom(){
	uint16_t values[PAD_COUNT];
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		values[i] = Pad::pads[i]->getVolume();
	}
	EEPROM.put(EEPROM_PAD_VOLUME, values);
}
