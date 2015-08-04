#include "MainMenu.h"

using namespace digitalcave;

MainMenu::MainMenu(){
	if (menuStack.size() == 0){
//		menuStack.push(this);
	}
}

void MainMenu::handleAction(){
	#define MENU_COUNT 4

	int8_t menuItem = encoder.read() / 2;
	if (menuItem < 0) encoder.write((MENU_COUNT - 1) * 2);		//Loop to end
	else if (menuItem >= MENU_COUNT) encoder.write(0);
	
	switch(menuItem){
		case 0:
			display.write_text(0, 0, "Main Volume         ", 20);
			if (button.fallingEdge()){
				down(MainVolume());
			}
			break;
		case 1:
			display.write_text(0, 0, "Channel Volume      ", 20);
			if (button.fallingEdge()){
				//TODO ChannelVolume object
				//ChannelVolume channelVolume();
				//down(channelVolume);
			}
			break;
		case 2:
			display.write_text(0, 0, "Load Samples        ", 20);
			if (button.fallingEdge()){
				down(LoadSamples());
			}
			break;
		case 3:
			display.write_text(0, 0, "Calibrate Channels  ", 20);
			if (button.fallingEdge()){
				//TODO CalilbrateChannels object
				//CalilbrateChannels calilbrateChannels();
				//down(calilbrateChannels);
			}
			break;
		default:
			//This should never happen...
			display.write_text(0, 0, "Unknown Menu Option ", 20);
			break;
	}
}
