#include "Menu.h"

using namespace digitalcave;

void Menu::handleCurrent(){
	menuStack.top().handleAction();
}

void Menu::up(){
	if (menuStack.size() > 1){
		menuStack.pop();
		encoder.write(menuStack.top().encoderState);
	}
}

void Menu::down(Menu newMenu) {
	encoderState = encoder.read();
	menuStack.push(newMenu);
	encoder.write(newMenu.encoderState);
}
