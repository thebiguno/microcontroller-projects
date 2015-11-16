#include "MainVolume.h"

using namespace digitalcave;

extern AudioControlSGTL5000 control;
extern Menu mainMenu;

MainVolume::MainVolume(){
	encoderState = 70;
	encoder.write(encoderState);
}

Menu* MainVolume::handleAction(){
	int8_t encoderVolume = encoder.read();
	if (volume * 100 != encoderVolume){
		if (encoderVolume > 100){
			encoderVolume = 100;
			encoder.write(100);
		}
		else if (encoderVolume < 0){
			encoderVolume = 0;
			encoder.write(0);
		}
		volume = encoderVolume / 100.0;
		Sample::setMasterVolume(volume);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume * 100));
	display.write_text(1, 0, buf, 4);
	
	if (button.fallingEdge()){
		display.write_text(1, 0, "    ", 4);
		return &mainMenu;
	}
	
	return NULL;
}
