#include "CalibratePads.h"

using namespace digitalcave;

extern Menu mainMenu;
extern CalibratePad calibratePad;

CalibratePads::CalibratePads(){
}

Menu* CalibratePads::handleAction(){
	int8_t pad = encoder.read() / 2;
	if (pad > SAMPLE_COUNT) encoder.write(0);
	else if (pad < 0) encoder.write(SAMPLE_COUNT * 2);
	
	switch(pad){
		case 0:
			display.write_text(1, 0, "Hi Hat              ", 20);
			break;
		case 1:
			display.write_text(1, 0, "Hi Hat Pedal        ", 20);
			break;
		case 2:
			display.write_text(1, 0, "Snare               ", 20);
			break;
		case 3:
			display.write_text(1, 0, "Bass                ", 20);
			break;
		case 4:
			display.write_text(1, 0, "Tom 1               ", 20);
			break;
		case 5:
			display.write_text(1, 0, "Crash               ", 20);
			break;
		case 6:
			display.write_text(1, 0, "Tom 2               ", 20);
			break;
		case 7:
			display.write_text(1, 0, "Tom 3               ", 20);
			break;
		case 8:
			display.write_text(1, 0, "Splash              ", 20);
			break;
		case 9:
			display.write_text(1, 0, "Ride                ", 20);
			break;
		case 10:
			display.write_text(1, 0, "X0                  ", 20);
			break;
		case 11:
			display.write_text(1, 0, "X1                  ", 20);
			break;
		case 12:
			display.write_text(1, 0, "<Cancel>            ", 20);
			break;
	}
	
	if (button.fallingEdge()){
		if (pad == SAMPLE_COUNT){
			display.write_text(1, 0, "                    ", 20);
			return &mainMenu;
		}
		else {
			calibratePad.value = -1;
			calibratePad.selectedPad = pad;
			return &calibratePad;
		}
	}

	return NULL;
}
