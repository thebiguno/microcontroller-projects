#include "MainVolume.h"
#include "../Sample.h"

using namespace digitalcave;

MainVolume::MainVolume(){
	encoderState = volume;
	encoder.write(encoderState);
}

Menu* MainVolume::handleAction(){
	int16_t encoderVolume = encoder.read();
	if (volume != encoderVolume){
		if (encoderVolume > 255){
			encoderVolume = 255;
			encoder.write(255);
		}
		else if (encoderVolume < 0){
			encoderVolume = 0;
			encoder.write(0);
		}
		volume = encoderVolume;
		Sample::setMasterVolume(volume);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / 256.0 * 100));
	display.write_text(1, 0, buf, 4);
	
	if (button.fallingEdge()){
		display.write_text(1, 0, "    ", 4);
		return Menu::mainMenu;
	}
	
	return NULL;
}
