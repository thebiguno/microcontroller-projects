#include "VolumePadSelect.h"

using namespace digitalcave;

VolumePadSelect::VolumePadSelect(){
}

Menu* VolumePadSelect::handleAction(){
	int8_t pad = encoder.read() / 2;
	if (pad >= (PAD_COUNT + 1)) encoder.write(0);
	else if (pad < 0) encoder.write(PAD_COUNT * 2);
	
	switch(pad){
		case 0:
			display->write_text(1, 0, "Hi Hat              ", 20);
			break;
		case 1:
			display->write_text(1, 0, "Snare               ", 20);
			break;
		case 2:
			display->write_text(1, 0, "Bass                ", 20);
			break;
		case 3:
			display->write_text(1, 0, "Tom 1               ", 20);
			break;
		case 4:
			display->write_text(1, 0, "Crash               ", 20);
			break;
		case 5:
			display->write_text(1, 0, "Tom 2               ", 20);
			break;
		case 6:
			display->write_text(1, 0, "Tom 3               ", 20);
			break;
		case 7:
			display->write_text(1, 0, "Splash              ", 20);
			break;
		case 8:
			display->write_text(1, 0, "Ride                ", 20);
			break;
		case 9:
			display->write_text(1, 0, "X0                  ", 20);
			break;
		case 10:
			display->write_text(1, 0, "X1                  ", 20);
			break;
		case 11:
			display->write_text(1, 0, "<Main Menu>         ", 20);
			break;
	}
	
	if (button.fallingEdge()){
		if (pad == PAD_COUNT){
			display->write_text(1, 0, "                    ", 20);
			return Menu::mainMenu;
		}
		else {
			((VolumePad*) Menu::volumePad)->value = -1;
			((VolumePad*) Menu::volumePad)->pad = pad;
			return Menu::volumePad;
		}
	}

	return NULL;
}
