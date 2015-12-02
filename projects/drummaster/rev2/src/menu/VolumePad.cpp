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
		if (encoderState > 255){
			encoderState = 255;
			encoder.write(255);
		}
		else if (encoderState < 0){
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
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		Pad::pads[i]->setVolume(EEPROM.read(EEPROM_PAD_VOLUME + i));
	}
}

void VolumePad::savePadVolumesToEeprom(){
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		EEPROM.update(EEPROM_PAD_VOLUME + i, Pad::pads[i]->getVolume());
	}
}
