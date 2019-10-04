#include "Display.h"

using namespace digitalcave;

extern Channel channels[CHANNEL_COUNT];
extern ButtonAVR button1;
extern ButtonAVR button2;

Display::Display() :
	hd44780(hd44780.FUNCTION_LINE_2 | hd44780.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7),
	char_display(&this->hd44780, DISPLAY_ROWS, DISPLAY_COLS) {

	char_display.clear();
	char_display.refresh();
}

void Display::update(State state){
	char_display.mark_dirty();
	hd44780.set_display(hd44780.DISPLAY_ON | hd44780.DISPLAY_CURSOR_OFF | hd44780.DISPLAY_BLINK_OFF);
	char buffer[DISPLAY_COLS + 1];
	uint8_t channel = state.get_scroll_channel();

	if (state.get_state() == STATE_LOCKED){
		for(uint8_t row = 0; row < DISPLAY_ROWS && row < CHANNEL_COUNT; row++){
			snprintf(buffer, DISPLAY_COLS + 1, "Ch %d %6.2fV %6.3fA ", row + 1, channels[row].get_voltage_actual() / 1000.0, channels[row].get_current_actual() / 1000.0);
			char_display.write_text(row, 0, buffer, DISPLAY_COLS);
		}

		//Clear any rows that are not occupied by channels, if there are any left.
		for (uint8_t row = CHANNEL_COUNT; row < DISPLAY_ROWS; row++){
			char_display.write_text(row, 0, "                    ", DISPLAY_COLS);
		}
	}
	else if (state.get_state() == STATE_EDIT_ITEM){
		snprintf(buffer, DISPLAY_COLS + 1, "   Channel %d         ", channel + 1);
		char_display.write_text(0, 0, buffer, DISPLAY_COLS);

		snprintf(buffer, DISPLAY_COLS + 1, "Set  %+6.2fV %6.3fA     ", channels[channel].get_voltage_setpoint() / 1000.0, channels[channel].get_current_setpoint() / 1000.0);
		char_display.write_text(1, 0, buffer, DISPLAY_COLS);

		snprintf(buffer, DISPLAY_COLS + 1, "Actl.%+6.2fV %6.3fA     ", channels[channel].get_voltage_actual() / 1000.0, channels[channel].get_current_actual() / 1000.0);
		char_display.write_text(2, 0, buffer, DISPLAY_COLS);

		char_display.write_text(3, 0, "                    ", DISPLAY_COLS);
		// char_display.write_text(1, 4, 0x7e);
		// char_display.write_text(1, 19, 0x7f);
	}
	else if (state.get_state() == STATE_MENU){
		char_display.write_text(0, 0, "  Calibrate Voltage ", DISPLAY_COLS);
		char_display.write_text(1, 0, "  Calibrate Current ", DISPLAY_COLS);
		char_display.write_text(2, 0, "  Startup Setpoints ", DISPLAY_COLS);
		char_display.write_text(3, 0, "                    ", DISPLAY_COLS);
		char_display.write_text(state.get_scroll_menu(), 0, 0x7e);
	}
	else if (state.get_state() == STATE_CALIBRATE_VOLTAGE || state.get_state() == STATE_CALIBRATE_CURRENT){
		if (state.get_calibrate_index() < CALIBRATION_COUNT){
			snprintf(buffer, DISPLAY_COLS + 1, "Channel %d (Cal. %d/%d)   ", channel + 1, state.get_calibrate_index() + 1, CALIBRATION_COUNT);
			char_display.write_text(0, 0, buffer, DISPLAY_COLS);

			if (state.get_state() == STATE_CALIBRATE_VOLTAGE){
				calibration_t calibration = channels[channel].get_calibration_voltage(state.get_calibrate_index());
				snprintf(buffer, DISPLAY_COLS + 1, "%+6.2fV DAC: %4d   ", calibration.actual / 1000.0, calibration.dac);
				char_display.write_text(1, 0, buffer, DISPLAY_COLS);
				snprintf(buffer, DISPLAY_COLS + 1, "        ADC: %4d   ", calibration.adc);
				char_display.write_text(2, 0, buffer, DISPLAY_COLS);
			}
			else {
				calibration_t calibration = channels[channel].get_calibration_current(state.get_calibrate_index());
				snprintf(buffer, DISPLAY_COLS + 1, "%+6.3fA DAC: %4d   ", calibration.actual / 1000.0, calibration.dac);
				char_display.write_text(1, 0, buffer, DISPLAY_COLS);
				snprintf(buffer, DISPLAY_COLS + 1, "        ADC: %4d   ", calibration.adc);
				char_display.write_text(2, 0, buffer, DISPLAY_COLS);
			}
			char_display.write_text(3, 0, "                    ", DISPLAY_COLS);
		}
		else {
			snprintf(buffer, DISPLAY_COLS + 1, "Channel %d (Complete)", channel + 1, state.get_calibrate_index() + 1, CALIBRATION_COUNT);
			char_display.write_text(0, 0, buffer, DISPLAY_COLS);

			char_display.write_text(1, 0, "                    ", DISPLAY_COLS);
			char_display.write_text(2, 0, "Hold R to Save      ", DISPLAY_COLS);
			char_display.write_text(3, 0, "Hold L to Edit      ", DISPLAY_COLS);
		}
	}
	else if (state.get_state() == STATE_STARTUP_SETPOINT){
		snprintf(buffer, DISPLAY_COLS + 1, "Channel %d (Startup) ", channel + 1);
		char_display.write_text(0, 0, buffer, DISPLAY_COLS);

		snprintf(buffer, DISPLAY_COLS + 1, "Set  %+6.2fV %6.3fA     ", state.get_voltage_setpoint() / 1000.0, state.get_current_setpoint() / 1000.0);
		char_display.write_text(1, 0, buffer, DISPLAY_COLS);


		char_display.write_text(2, 0, "Hold R to Save      ", DISPLAY_COLS);
		char_display.write_text(3, 0, "Hold L to Abort    ", DISPLAY_COLS);
	}
	else if (state.get_state() == STATE_SET_AREF_SOURCE){
		char_display.write_text(0, 0, "Configure DAC 2.3 to", DISPLAY_COLS);
		char_display.write_text(1, 0, "output 5V for AREF. ", DISPLAY_COLS);
		char_display.write_text(2, 0, "Hold R = Confirm    ", DISPLAY_COLS);
		char_display.write_text(3, 0, "Hold L = Cancel     ", DISPLAY_COLS);
	}
	else if (state.get_state() == STATE_FIRMWARE){
		char_display.write_text(0, 0, "Upload new Firmware.", DISPLAY_COLS);
		char_display.write_text(1, 0, "                    ", DISPLAY_COLS);
		char_display.write_text(2, 0, "      Restart       ", DISPLAY_COLS);
		char_display.write_text(3, 0, "   when complete.   ", DISPLAY_COLS);
	}

	char_display.refresh();
}

void Display::force_reset(){
	char_display.clear();
}

CharDisplay Display::get_char_display(){
	return char_display;
}
