#include "State.h"

using namespace digitalcave;

extern AudioControlSGTL5000 control;

State::State() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7),
	display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS),
	encoder(ENC_A, ENC_B),
	button(ENC_PUSH, 25)
{
	pinMode(ENC_PUSH, INPUT_PULLUP);	//Encoder pushbutton	
}

void State::poll(){
	button.update();

	char buf[21];
	if (state == STATE_MENU){
		int8_t encoder_state = encoder.read() / 2;
		if (encoder_state < 0) encoder.write((MENU_COUNT - 1) * 2);
		else if (encoder_state >= MENU_COUNT) encoder.write(0);
		
		switch(encoder_state){
			case MENU_MAIN_VOLUME:
				display.write_text(0, 0, "Main Volume         ", 20);
				if (button.fallingEdge()){
					encoder.write(volume * 100);
					state = STATE_MAIN_VOLUME;
				}
				break;
			case MENU_CHANNEL_VOLUME:
				display.write_text(0, 0, "Channel Volume      ", 20);
				if (button.fallingEdge()){
					state = STATE_CHANNEL_VOLUME;
				}
				break;
			case MENU_LOAD_SAMPLES:
				display.write_text(0, 0, "Load Samples        ", 20);
				if (button.fallingEdge()){
					state = STATE_SCAN_FOLDERS;
					encoder.write(0);
				}
				break;
			case MENU_CALIBRATE:
				display.write_text(0, 0, "Calibrate Channels  ", 20);
				if (button.fallingEdge()){
					state = STATE_CALIBRATION;
					encoder.write(0);
				}
				break;
			default:
				display.write_text(0, 0, "Unknown Menu Option ", 20);
				break;
		}
	}
	else if (state == STATE_MAIN_VOLUME){
		int8_t encoder_state = encoder.read();
		if (volume * 100 != encoder_state){
			if (encoder_state > 100){
				encoder_state = 100;
				encoder.write(100);
			}
			else if (encoder_state < 0){
				encoder_state = 0;
				encoder.write(0);
			}
			volume = encoder_state / 100.0;
			control.volume(volume);
		}
		snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume * 100));
		display.write_text(1, 0, buf, 4);
		
		if (button.fallingEdge()){
			state = STATE_MENU;
			encoder_state = MENU_MAIN_VOLUME * 2;
			display.write_text(1, 0, "    ", 4);
		}
	}
	else if (state == STATE_CHANNEL_VOLUME){
		state = STATE_MENU; //TODO
	}
	else if (state == STATE_SCAN_FOLDERS){
		/*
		static std::vector<String> folders = getTopLevelFolders();
		
		int8_t encoder_state = encoder.read() / 2;
		if (encoder_state < 0){
			encoder_state = folders.size() - 1;
			encoder.write((folders.size() - 1) * 2);
		}
		else if (encoder_state >= (int32_t) folders.size()){
			encoder_state = 0;
			encoder.write(0);
		}
		
		snprintf(buf, sizeof(buf), "%s                   ", folders[encoder_state].c_str());
		display.write_text(1, 0, buf, 20);
		
		if (button.fallingEdge()){
			if (encoder_state == 0){
				//Cancel option (first in the list)
				encoder.write(MENU_LOAD_SAMPLES * 2);
			}
			else {
				copyFolderToFlash(folders[encoder_state].c_str(), display);
				encoder.write(MENU_MAIN_VOLUME * 2);
			}
			state = STATE_MENU;
			display.clear();
			display.mark_dirty();
		}
		*/
	}
	else if (state == STATE_CALIBRATION){
		int8_t encoder_state = encoder.read() / 2;
		if (encoder_state < 0) encoder.write((CHANNEL_COUNT - 1) * 2);
		else if (encoder_state >= CHANNEL_COUNT) encoder.write(0);

		snprintf(buf, sizeof(buf), "Ch. %2d     ", encoder_state);
		display.write_text(1, 0, buf, 8);
		
		if (button.fallingEdge()){
			//Start calibration routine
			calibrateChannel(encoder_state, display, button);
		}
	}
	else {
		snprintf(buf, sizeof(buf), "State %d                    ", (uint8_t) this->get_state());
		display.write_text(0, 0, buf, sizeof(buf));
	}
	
	display.refresh();
}

uint8_t State::get_state(){
	return state;
}

float State::get_volume(){
	return volume;
}
