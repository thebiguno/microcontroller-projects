#include "Stats.h"
#include <Audio.h>
#include "../hardware.h"

using namespace digitalcave;

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

Stats::Stats() : Menu(1), lastUpdate(0), forceUpdate(1) {
}

Menu* Stats::handleAction(){
	display->write_text(0, 0, "System Status        ", 20);
	
	if (millis() - lastUpdate > 5000 || forceUpdate){
		snprintf(buf, sizeof(buf), "CPU: %3d%% Mem: %3d%%      ", (uint8_t) AudioProcessorUsage(), (uint8_t) ((double) AudioMemoryUsage() / AUDIO_MEMORY * 100));
		display->write_text(1, 0, buf, 20);
		snprintf(buf, sizeof(buf), "Max: %3d%% Max: %3d%%      ", (uint8_t) AudioProcessorUsageMax(), (uint8_t) ((double) AudioMemoryUsageMax() / AUDIO_MEMORY * 100));
		display->write_text(2, 0, buf, 20);
		snprintf(buf, sizeof(buf), "Version: %s                ", STRINGIFY(GIT_VERSION));
		display->write_text(3, 0, buf, 20);
		lastUpdate = millis();
		forceUpdate = 0;
	}
	
	if (button.releaseEvent() || button.longPressEvent()){
		display->clear();
		forceUpdate = 1;
		return Menu::mainMenu;
	}
	
	return NULL;
}
