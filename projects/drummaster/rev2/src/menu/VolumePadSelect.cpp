#include "VolumePadSelect.h"

using namespace digitalcave;

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

VolumePadSelect::VolumePadSelect() : Menu(PAD_COUNT){
}

Menu* VolumePadSelect::handleAction(){
	display->write_text(0, 0, "Calibrate Channels   ", 20);
	
	display->write_text(2, 0, (char) 0x7E);
	display->write_text(1, 1, labels[getMenuPosition(-1)], 19);
	display->write_text(2, 1, labels[getMenuPosition()], 19);
	display->write_text(3, 1, labels[getMenuPosition(1)], 19);
	
	if (button.releaseEvent()){
		((VolumePad*) Menu::volumePad)->value = -1;
		((VolumePad*) Menu::volumePad)->pad = getMenuPosition();
		return Menu::volumePad;
	}
	else if (button.longPressEvent()){
		display->clear();
		return Menu::mainMenu;
	}

	return NULL;
}
