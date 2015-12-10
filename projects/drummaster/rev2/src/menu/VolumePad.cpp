#include "VolumePad.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(256.0 / 5)

VolumePad::VolumePad() : value(-1), pad(0) {
}

void VolumePad::loadPadVolumesFromEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	Serial.print("Loading volumes for kit index = ");
	Serial.println(kitIndex);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		Pad::pads[i]->setPadVolume(EEPROM.read(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex)) / GAIN_DIVISOR);
		Serial.print("Loading value ");
		Serial.print(EEPROM.read(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex)));
		Serial.print(" from address ");
		Serial.println(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex));
	}
}

void VolumePad::savePadVolumesToEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	Serial.print("Saving volumes for kit index = ");
	Serial.println(kitIndex);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		Serial.print("Saving value ");
		Serial.print(Pad::pads[i]->getPadVolume() * GAIN_DIVISOR);
		Serial.print(" to address ");
		Serial.println(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex));
		EEPROM.update(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex), Pad::pads[i]->getPadVolume() * GAIN_DIVISOR);
	}
}

Menu* VolumePad::handleAction(){
	if (value == -1){
		value = Pad::pads[pad]->getPadVolume() * GAIN_DIVISOR;
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
		Pad::pads[pad]->setPadVolume(value / GAIN_DIVISOR);
	}

	snprintf(buf, sizeof(buf), "%d%%      ", (uint16_t) (value / GAIN_DIVISOR * 100));
	display->write_text(2, 0, buf, 5);

	if (button.fallingEdge()){
		EEPROM.update(EEPROM_PAD_VOLUME + pad, (uint8_t) value);
		display->write_text(2, 0, "                    ", 20);
		return Menu::volumePadSelect;
	}
	
	return NULL;
}

