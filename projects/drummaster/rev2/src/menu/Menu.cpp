#include "Menu.h"
#include "MainMenu.h"

using namespace digitalcave;

//Initialize static member variables
Hd44780_Teensy Menu::hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
CharDisplay Menu::display(&hd44780, DISPLAY_ROWS, DISPLAY_COLS);
Encoder Menu::encoder(ENC_A, ENC_B);
Bounce Menu::button(ENC_PUSH, 25);
char Menu::buf[21];
std::vector<Menu*> Menu::menuStack;

// Menu::Menu(){
// 	//Do nothing; implemented in sub class
// }

// void Menu::handleAction(){
// 	//Do nothing; implemented in sub class
// 	Serial.println("Menu.cpp");
// }

void Menu::poll(){
	if (menuStack.size() == 0){
		menuStack.push_back(new MainMenu());
	}
	menuStack.back()->handleAction();
	Serial.println((uint32_t) &(menuStack.back()));
	display.refresh();
}

void Menu::up(){
	if (menuStack.size() > 1){
		menuStack.pop_back();
		encoder.write(menuStack.back()->encoderState);
	}
}

void Menu::down(Menu* newMenu) {
	encoderState = encoder.read();
	menuStack.push_back(newMenu);
	encoder.write(newMenu->encoderState);
}

namespace std {
  void __throw_bad_alloc(){
    Serial.println("Unable to allocate memory");
	while(1);
  }

  void __throw_length_error( char const*e ){
    Serial.print("Length Error :");
    Serial.println(e);
	while(1);
  }
}