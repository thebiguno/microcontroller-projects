#include "VolumePad.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(256.0 / 5)

static const char* labels[PAD_COUNT] = {
	"Hi Hat             ",
	"Snare              ",
	"Bass               ",
	"Tom 1              ",
	"Crash              ",
	"Tom 2              ",
	"Tom 3              ",
	"Splash             ",
	"Ride               ",
	"X0                 ",
	"X1                 "
};

VolumePad::VolumePad() : Menu(PAD_COUNT), value(-1), pad(0) {
}

void VolumePad::loadPadVolumesFromEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		Pad::pads[i]->setPadVolume(EEPROM.read(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex)) / GAIN_DIVISOR);
	}
}

void VolumePad::savePadVolumesToEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		EEPROM.update(EEPROM_PAD_VOLUME + i + (PAD_COUNT * kitIndex), Pad::pads[i]->getPadVolume() * GAIN_DIVISOR);
	}
}

Menu* VolumePad::handleAction(){
	display->write_text(0, 0, "Calibrate Channels   ", 20);
	display->write_text(1, 0, labels[pad], 20);
	
	if (value == -1){
		value = Pad::pads[pad]->getPadVolume() * GAIN_DIVISOR;
		setMenuPosition(value);
	}
	
	snprintf(buf, sizeof(buf), "%d%%                ", (uint16_t) (value / GAIN_DIVISOR * 100));
	display->write_text(2, 0, buf, 20);
	
	encoderState = getMenuPosition();
	if (getMenuPosition() != value){
		value = getMenuPosition();
		Pad::pads[pad]->setPadVolume(value / GAIN_DIVISOR);
	}

	if (button.releaseEvent()){
		savePadVolumesToEeprom();
		display->clearRow(2);
		return Menu::volumePadSelect;
	}
	
	return NULL;
}

