#include "State.h"

using namespace digitalcave;

State::State() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7),
	char_display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS),
	encoder(ENC_A, ENC_B),
	button(ENC_PUSH, 25)
{
	pinMode(ENC_PUSH, INPUT_PULLUP);	//Encoder pushbutton
}

void State::poll(){
	button.update();
	int8_t encoder_state = encoder.read();
	if (encoder_state < 0) encoder.write(ENCODER_STATE_COUNT - 1);
	else if (encoder_state >= ENCODER_STATE_COUNT) encoder.write(0);

	this->state = encoder.read() >> 1;

	char buf[20];
	if (this->state == 1 && this->button.fallingEdge()){
		if (!SD.begin(CS_SD)) {
			char_display.clear();
			char_display.write_text(0, 0, "SD Error", 8);
			char_display.refresh();
			delay(1000);
			return;
		}
		SerialFlash.begin(CS_FLASH);
		uint8_t id[3];
		SerialFlash.readID(id);
		uint32_t size = SerialFlash.capacity(id);
		
		char_display.clear();
		snprintf(buf, sizeof(buf), "Erasing %dMB...", (uint16_t) (size >> 20));
		char_display.write_text(0, 0, buf, sizeof(buf));
		char_display.refresh();
		
		SerialFlash.eraseAll();
		
		//Show progress while erasing...
		uint32_t last_millis = millis();
		uint8_t row = 1;
		uint8_t col = 0;
		while (!SerialFlash.ready()) {
			if (millis() - last_millis > 2000) {
				last_millis = millis();
				char_display.write_text(row, col, '.');
				col++;
				if (col >= DISPLAY_COLS){
					col = 0;
					row++;
				}
				char_display.refresh();
			}
		}

		char_display.clear();
		snprintf(buf, sizeof(buf), "Copy SD...             ");
		char_display.write_text(0, 0, buf, sizeof(buf));
		char_display.refresh();
		
		File rootdir = SD.open("/");
		while (1) {
			// open a file from the SD card
			File f = rootdir.openNextFile();
			if (!f) break;
			const char *filename = f.name();
			uint32_t length = f.size();
			snprintf(buf, sizeof(buf), "%s                       ", filename);
			char_display.write_text(1, 0, buf, sizeof(buf));
			char_display.refresh();
			
			// Create the file on the Flash chip and copy data
			if (SerialFlash.create(filename, length)) {
				SerialFlashFile ff = SerialFlash.open(filename);
				if (ff) {
					// copy data loop
					uint32_t count = 0;
					while (count < length) {
						uint8_t buf[256];
						uint16_t n;
						n = f.read(buf, 256);
						ff.write(buf, n);
						count = count + n;
					}
					ff.close();
				} 
				else {
					char_display.clear();
					char_display.write_text(0, 0, "Flash Error Open", 17);
					char_display.refresh();
					delay(1000);
					return;
				}
			}
			else {
				char_display.clear();
				char_display.write_text(0, 0, "Flash Error Create", 18);
				char_display.refresh();
				delay(1000);
				return;
			}
			f.close();
		}
		rootdir.close();
	}
	else {
		snprintf(buf, sizeof(buf), "State %d                   ", (uint8_t) this->get_state());
		char_display.write_text(0, 0, buf, sizeof(buf));
	}
	
	char_display.refresh();
}

uint8_t State::get_state(){
	return this->state;
}
