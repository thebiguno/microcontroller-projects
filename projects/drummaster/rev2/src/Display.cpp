#include "Display.h"

using namespace digitalcave;

Display::Display() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7),
	char_display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS) {

	char_display.clear();
	char_display.refresh();
}

void Display::update(State state){
	//TODO Update char_display according to state
	char buf[20];
	
	snprintf(buf, sizeof(buf), "%d                                  ", state.get_state());
	char_display.write_text(0, 0, buf, sizeof(buf));
	char_display.refresh();
}

void Display::force_reset(){
	char_display.clear();
}

CharDisplay Display::get_char_display(){
	return char_display;
}