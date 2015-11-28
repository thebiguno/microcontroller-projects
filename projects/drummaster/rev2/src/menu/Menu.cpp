#include "Menu.h"

#include "MainMenu.h"
#include "MainVolume.h"
#include "LineInVolume.h"
#include "PadsVolume.h"
#include "LoadSamples.h"
#include "CalibratePads.h"
#include "CalibratePad.h"

using namespace digitalcave;

//Initialize static member variables
char Menu::buf[21];
Hd44780_Teensy Menu::hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
CharDisplay Menu::display(&hd44780, DISPLAY_ROWS, DISPLAY_COLS);
Encoder Menu::encoder(ENC_A, ENC_B);
Bounce Menu::button(ENC_PUSH, 25);

//Initialize static references to menu items
Menu* Menu::mainMenu = new MainMenu();
Menu* Menu::mainVolume = new MainVolume();
Menu* Menu::lineInVolume = new LineInVolume();
Menu* Menu::padsVolume = new PadsVolume();
Menu* Menu::loadSamples = new LoadSamples();
Menu* Menu::calibratePads = new CalibratePads();
Menu* Menu::calibratePad = new CalibratePad;

Menu* Menu::current = Menu::mainMenu;

void Menu::poll(){
	current->button.update();
	Menu* newMenu = current->handleAction();
	current->display.refresh();
	if (newMenu != NULL){
		change(newMenu);
	}
}

void Menu::change(Menu* newMenu){
	current->encoderState = current->encoder.read();
	current = newMenu;
	newMenu->encoder.write(newMenu->encoderState);
}
