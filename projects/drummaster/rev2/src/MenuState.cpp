#include "MenuState.h"

using namespace digitalcave;

MenuState::MenuState(Menu* menu){
	current = menu;
}

void MenuState::poll(){
	current->button.update();
	Menu* newMenu = current->handleAction();
	current->display.refresh();
	if (newMenu != NULL){
		change(newMenu);
	}
}

void MenuState::change(Menu* newMenu){
	current->encoderState = current->encoder.read();
	current = newMenu;
	newMenu->encoder.write(newMenu->encoderState);
}
