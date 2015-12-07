#include "Stats.h"
#include <Audio.h>
#include "../hardware.h"

using namespace digitalcave;

Stats::Stats() : lastUpdate(0), forceUpdate(1) {
}

Menu* Stats::handleAction(){
	if (millis() - lastUpdate > 5000 || forceUpdate){
		snprintf(buf, sizeof(buf), "CPU: %3d%% Mem: %3d%%      ", (uint8_t) AudioProcessorUsage(), (uint8_t) ((double) AudioMemoryUsage() / AUDIO_MEMORY * 100));
		display->write_text(1, 0, buf, 20);
		snprintf(buf, sizeof(buf), "Max: %3d%% Max: %3d%%      ", (uint8_t) AudioProcessorUsageMax(), (uint8_t) ((double) AudioMemoryUsageMax() / AUDIO_MEMORY * 100));
		display->write_text(2, 0, buf, 20);
		lastUpdate = millis();
		forceUpdate = 0;
	}
	
	if (button.fallingEdge()){
		display->clear();
		forceUpdate = 1;
		return Menu::mainMenu;
	}
	
	return NULL;
}
