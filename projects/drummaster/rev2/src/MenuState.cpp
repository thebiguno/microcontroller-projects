#include "MenuState.h"

using namespace digitalcave;

MenuState::MenuState(Menu* menu){
	current = menu;
}

void MenuState::poll(){
	current->button.update();
	current->handleAction();
	current->display.refresh();
}

void MenuState::change(Menu* newMenu){
	current->encoderState = current->encoder.read();
	current = newMenu;
	newMenu->encoder.write(newMenu->encoderState);
}
