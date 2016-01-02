#include "VolumePadSelect.h"

using namespace digitalcave;

static const char* labels[] = {
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

VolumePadSelect::VolumePadSelect() : Menu(PAD_COUNT){
}

Menu* VolumePadSelect::handleAction(){
	display->write_text(0, 0, "Pad Volume Adjust   ", 20);
	
	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	display->write_text(1, 1, labels[getMenuPosition(positionOffset - 1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(positionOffset)], 19);
	display->write_text(3, 1, labels[getMenuPosition(positionOffset + 1)], 19);
	
	if (button.releaseEvent()){
		((VolumePad*) Menu::volumePad)->value = -1;
		((VolumePad*) Menu::volumePad)->pad = getMenuPosition(0);
		return Menu::volumePad;
	}
	else if (button.longPressEvent()){
		display->clear();
		return Menu::settings;
	}

	return NULL;
}
