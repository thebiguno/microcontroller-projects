#include "Menu.h"
#include "MainMenu.h"

using namespace digitalcave;

//Initialize static member variables
Hd44780_Teensy Menu::hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
CharDisplay Menu::display(&hd44780, DISPLAY_ROWS, DISPLAY_COLS);
Encoder Menu::encoder(ENC_A, ENC_B);
Bounce Menu::button(ENC_PUSH, 25);
char Menu::buf[21];
std::vector<Menu> Menu::menuStack;

void Menu::poll(){
	Serial.println(menuStack.size());
	if (menuStack.size() > 0){
		menuStack.back().handleAction();
	}
	display.refresh();
}

void Menu::up(){
	if (menuStack.size() > 1){
		menuStack.pop_back();
		encoder.write(menuStack.back().encoderState);
	}
}

void Menu::down(Menu newMenu) {
	encoderState = encoder.read();
	menuStack.push_back(newMenu);
	encoder.write(newMenu.encoderState);
}
