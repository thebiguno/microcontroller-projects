#include "CalibrateChannelSelect.h"

using namespace digitalcave;

static const char* labels[] = {
	"Hi Hat             ",
	"Hi Hat Pedal       ",
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

CalibrateChannelSelect::CalibrateChannelSelect() : Menu(CHANNEL_COUNT){
}

Menu* CalibrateChannelSelect::handleAction(){
	display->write_text(0, 0, "Calibrate Channels   ", 20);
	
	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	display->write_text(1, 1, labels[getMenuPosition(positionOffset - 1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(positionOffset)], 19);
	display->write_text(3, 1, labels[getMenuPosition(positionOffset + 1)], 19);
	
	if (button.releaseEvent()){
		((CalibrateChannel*) Menu::calibrateChannel)->value = -1;
		((CalibrateChannel*) Menu::calibrateChannel)->channel = getMenuPosition(0);
		return Menu::calibrateChannel;
	}
	else if (button.longPressEvent()){
		display->clear();
		return Menu::mainMenu;
	}

	return NULL;
}
