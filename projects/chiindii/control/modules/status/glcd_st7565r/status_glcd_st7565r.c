#include <stdio.h>

#include "../status.h"

#include "../../../lib/draw/draw.h"
#include "../../../lib/draw/glcd/glcd.h"
#include "../../../lib/draw/glcd/glcd_st7565r.h"
#include "../../../lib/draw/fonts/ascii.h"
#include "../../../lib/draw/fonts/xsmall.h"
#include "../../../lib/draw/fonts/small.h"
#include "../../../lib/draw/fonts/medium.h"
#include "../../../lib/draw/fonts/xlarge.h"

//Flight mode layout
#define STATUS_THROTTLE_X			0
#define STATUS_THROTTLE_Y			0
#define STATUS_PITCH_X				39
#define STATUS_PITCH_Y				0
#define STATUS_ROLL_X				39
#define STATUS_ROLL_Y				17
#define STATUS_TIME_X				1
#define STATUS_TIME_Y				21
#define STATUS_BATTERY_PILOT_X		68
#define STATUS_BATTERY_PILOT_Y		0
#define STATUS_BATTERY_CONTROL_X	116
#define STATUS_BATTERY_CONTROL_Y	0
#define STATUS_COMM_X				82
#define STATUS_COMM_Y				0
#define STATUS_MOTORS_X				82
#define STATUS_MOTORS_Y				0

#define STATUS_CONFIG_MODE_TITLE_X		32
#define STATUS_CONFIG_MODE_TITLE_Y		0
#define STATUS_CONFIG_MODE_TEXT_X		0
#define STATUS_CONFIG_MODE_TEXT_Y		8
#define STATUS_CONFIG_MODE_COLUMN_WIDTH	31
#define STATUS_CONFIG_MODE_ROW_HEIGHT	8

#define VALUE_ON	1

//Temp buffer for printf
char temp[16];

/*
 * Initializes the persistent text / graphics for each mode
 */
void status_init_mode_flight(){
	//Clear buffer
	draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	
	//Write text labels
	draw_text(STATUS_PITCH_X, STATUS_PITCH_Y, (char*) "Pitch", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_ROLL_X, STATUS_ROLL_Y, (char*) "Roll", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps,  1,OVERLAY_OR);
	
	// degree symbols
	draw_rectangle(63,  6, 65,  8, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(63, 23, 65, 25, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	
	//Draw invalid batteries
	status_set_pilot_battery_level(-1);
	status_set_control_battery_level(-1);
	
	glcd_write_buffer();
}

void status_init_mode_calibrate(){
	//Clear buffer
	draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write text labels
	draw_text(STATUS_CONFIG_MODE_TITLE_X, STATUS_CONFIG_MODE_TITLE_Y, (char*) "Calibration", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, ORIENTATION_NORMAL, font_small, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Ensure craft is level and press", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 2 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "triangle to calibrate", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	
	
	glcd_write_buffer();
}

void status_init_mode_version(){
	//Clear buffer
	draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write text labels
	draw_text(STATUS_CONFIG_MODE_TITLE_X, STATUS_CONFIG_MODE_TITLE_Y, (char*) "Version", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, ORIENTATION_NORMAL, font_small, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	
	glcd_write_buffer();
}

void status_init_mode_pid(){
	//Clear buffer
	draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write text labels
	draw_text(STATUS_CONFIG_MODE_TITLE_X, STATUS_CONFIG_MODE_TITLE_Y, (char*) "PID Tuning", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, ORIENTATION_NORMAL, font_small, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "P", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 2 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "I", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 3 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "D", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);

	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Pitch", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 2 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Roll", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	
	
	glcd_write_buffer();
}

void status_init_mode_kalman(){
	//TODO set up kalman mode
	//Clear buffer
	draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write text labels
	draw_text(STATUS_CONFIG_MODE_TITLE_X, STATUS_CONFIG_MODE_TITLE_Y, (char*) "Kalman Tuning", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, ORIENTATION_NORMAL, font_small, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "alpha", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 2 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "bias", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 3 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 0 * STATUS_CONFIG_MODE_ROW_HEIGHT, (char*) "sz", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);

	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Pitch", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 2 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Roll", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	
	
	glcd_write_buffer();
}

void status_init(){
	st7565r_init(&PORT_LCD_DATA, PIN_LCD_DATA, 
			&PORT_LCD_CLOCK, PIN_LCD_CLOCK, 
			&PORT_LCD_A0, PIN_LCD_A0, 
			&PORT_LCD_RESET, PIN_LCD_RESET);

}


void status_set_battery_level(double voltage, uint8_t x, uint8_t y, uint8_t pilot){
	//Clear existing
	draw_rectangle(x, y, x + 11, y + 31, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	
	//Redraw edge
	draw_rectangle(x, y + 1, x + 11, y + 31, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	draw_line(x + 3, y, x + 8, y, VALUE_ON, OVERLAY_OR);
	
	//Fill according to level
	if (voltage < 0.0){
		//Draw an X for invalid values
		draw_line(x, y + 1, x + 11, y + 31, VALUE_ON, OVERLAY_OR);
		draw_line(x + 11, y + 1, x, y + 31, VALUE_ON, OVERLAY_OR);
	}
	else {
		//We need to convert from voltage to a graph.  For the pilot we assume, 10V is empty, 11.3V is full.  There are 31 lines 
		// to work with here, so we apply the following logic:
		// 11.3 - 10 = 1.3 (so we have a valid range of 1.7V)
		// 1.3 / 31 ~= 0.04194V / line
		//So, when computing the lines we take raw battery value, subtract 10, and multiply by 0.0448 to determine
		// how many lines it should cover.
		//
		//For the controller, the same logic applies, but with smaller numbers: max 4.8, min 4.0.
		// 4.8 - 4.0 - 0.8
		// 0.8 / 31 ~= 0.026V / line
		int8_t lines = (pilot ? (uint8_t) ((voltage - 10) / 0.04194) : (uint8_t) ((voltage - 4) / 0.026));
		if (lines > 31) lines = 31;
		if (lines < 0) lines = 0;
		draw_rectangle(x + 1, y + 33 - lines, x + 10, y + 31, DRAW_FILLED, VALUE_ON, OVERLAY_OR);

		//Print name
		draw_text(x + 2, y + 25, (char*) (pilot ? "P" : "C"), FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_XOR);
		
		//Print voltage CW 90 degrees
		sprintf(temp, (char*) "%4.1f", voltage);
		draw_text(x + 3, y + 7, temp, FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_DOWN, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_XOR);
	}
	
	//Flush
	glcd_write_buffer_bounds(x, y, x + 11, y + 32);
}

void status_set_pilot_battery_level(double voltage){
	status_set_battery_level(voltage, STATUS_BATTERY_PILOT_X, STATUS_BATTERY_PILOT_Y, 1);
}

void status_set_control_battery_level(double voltage){
	status_set_battery_level(voltage, STATUS_BATTERY_CONTROL_X, STATUS_BATTERY_CONTROL_Y, 0);
}

void status_set_telemetry(double pitch, double roll){
	//Clear existing
	draw_rectangle(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 23, STATUS_PITCH_Y + 15, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	draw_rectangle(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 23, STATUS_ROLL_Y + 15, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);	

	//Write values
	if (pitch > 1000){
		draw_text(STATUS_PITCH_X, STATUS_PITCH_Y + 6, (char*) "---", FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);		
	}
	else {
		sprintf(temp, (char*) "%3.1d", (int8_t) (pitch * 57.2957795));
		draw_text(STATUS_PITCH_X, STATUS_PITCH_Y + 6, temp, FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	
	}
	
	if (roll > 1000){
		draw_text(STATUS_ROLL_X, STATUS_ROLL_Y + 6, (char*) "---", FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);		
	}
	else {
		sprintf(temp, (char*) "%3.1d", (int8_t) (roll * 57.2957795));
		draw_text(STATUS_ROLL_X, STATUS_ROLL_Y + 6, temp, FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	
	}
	
	//Flush
	glcd_write_buffer_bounds(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 23, STATUS_PITCH_Y + 15);
	glcd_write_buffer_bounds(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 23, STATUS_ROLL_Y + 15);
}

void status_set_throttle(double throttle, uint8_t armed){
	glcd_set_display_inverted(armed);

	//Clear existing
	draw_rectangle(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, STATUS_THROTTLE_X + 34, STATUS_THROTTLE_Y + 16, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	
	if (armed){
		if (throttle > 1) throttle = 1;
		if (throttle < 0) throttle = 0;
	
		//Write values
		sprintf(temp, (char*) "%3.2d", (int8_t) (throttle * 100));
		draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, temp, FONT_XLARGE_WIDTH, FONT_XLARGE_HEIGHT, ORIENTATION_NORMAL, font_xlarge, codepage_xlarge, VALUE_ON, OVERLAY_OR);
	}
	else {
		draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, (char*) "---", FONT_XLARGE_WIDTH, FONT_XLARGE_HEIGHT, ORIENTATION_NORMAL, font_xlarge, codepage_xlarge, VALUE_ON, OVERLAY_OR);
	}

	//Flush
	glcd_write_buffer_bounds(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, STATUS_THROTTLE_X + 34, STATUS_THROTTLE_Y + 16);
}

void status_set_armed_time(uint32_t millis){
	//Clear existing
	draw_rectangle(STATUS_TIME_X, STATUS_TIME_Y, STATUS_TIME_X + 32, STATUS_TIME_Y + 10, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write values
	sprintf(temp, (char*) "%02d", (uint8_t) (millis / 60000));
	draw_text(STATUS_TIME_X, STATUS_TIME_Y, temp, FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	sprintf(temp, (char*) "%02d", (uint8_t) ((millis % 60000) / 1000));
	draw_text(STATUS_TIME_X + 18, STATUS_TIME_Y, temp, FONT_MEDIUM_WIDTH, FONT_MEDIUM_HEIGHT, ORIENTATION_NORMAL, font_medium, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	
	draw_line(17, 23, 17, 25, VALUE_ON, OVERLAY_OR);
	draw_line(17, 28, 17, 30, VALUE_ON, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_TIME_X, STATUS_TIME_Y, STATUS_TIME_X + 32, STATUS_TIME_Y + 10);
}

void status_set_comm_state(uint8_t tx, uint8_t rx){
	//Clear existing
	draw_rectangle(STATUS_COMM_X, STATUS_COMM_Y, STATUS_COMM_X + 6, STATUS_COMM_Y + 11, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);

	//Write values
	if (tx) draw_text(STATUS_COMM_X, STATUS_COMM_Y, (char*) "TX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	if (rx) draw_text(STATUS_COMM_X, STATUS_COMM_Y + 7, (char*) "RX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_COMM_X, STATUS_COMM_Y, STATUS_COMM_X + 6, STATUS_COMM_Y + 10);
}

void status_set_motors(double left, double front, double right, double back){
	uint8_t lx = 0; uint8_t ly = 14;
	uint8_t fx = 14; uint8_t fy = 0;
	uint8_t rx = 18; uint8_t ry = 14;
	uint8_t bx = 14; uint8_t by = 18;
	uint8_t sm = 3; uint8_t lg = 13;
	
	//Clear existing
	draw_rectangle(STATUS_MOTORS_X + lx, STATUS_MOTORS_Y + ly, STATUS_MOTORS_X + lx + lg, STATUS_MOTORS_Y + ly + sm, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	draw_rectangle(STATUS_MOTORS_X + fx, STATUS_MOTORS_Y + fy, STATUS_MOTORS_X + fx + sm, STATUS_MOTORS_Y + fy + lg, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	draw_rectangle(STATUS_MOTORS_X + rx, STATUS_MOTORS_Y + ry, STATUS_MOTORS_X + rx + lg, STATUS_MOTORS_Y + ry + sm, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
	draw_rectangle(STATUS_MOTORS_X + bx, STATUS_MOTORS_Y + by, STATUS_MOTORS_X + bx + sm, STATUS_MOTORS_Y + by + lg, DRAW_FILLED, VALUE_ON, OVERLAY_NAND);
 
	//Redraw edges: left, top, right, bottom
	draw_rectangle(STATUS_MOTORS_X + lx, STATUS_MOTORS_Y + ly, STATUS_MOTORS_X + lx + lg, STATUS_MOTORS_Y + ly + sm, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + fx, STATUS_MOTORS_Y + fy, STATUS_MOTORS_X + fx + sm, STATUS_MOTORS_Y + fy + lg, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + rx, STATUS_MOTORS_Y + ry, STATUS_MOTORS_X + rx + lg, STATUS_MOTORS_Y + ry + sm, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + bx, STATUS_MOTORS_Y + by, STATUS_MOTORS_X + bx + sm, STATUS_MOTORS_Y + by + lg, DRAW_UNFILLED, VALUE_ON, OVERLAY_OR);
	
	//Fill according to motor levels
	draw_rectangle(STATUS_MOTORS_X + lx + lg - (uint8_t) (left * 12), STATUS_MOTORS_Y + ly, STATUS_MOTORS_X + lx + lg, STATUS_MOTORS_Y + ly + sm, DRAW_FILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + fx, STATUS_MOTORS_Y + fy + lg - (uint8_t) (front * 12), STATUS_MOTORS_X + fx + sm, STATUS_MOTORS_Y + fy + lg, DRAW_FILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + rx, STATUS_MOTORS_Y + ry, STATUS_MOTORS_X + rx + (uint8_t) (right * 12), STATUS_MOTORS_Y + ry + sm, DRAW_FILLED, VALUE_ON, OVERLAY_OR);
	draw_rectangle(STATUS_MOTORS_X + bx, STATUS_MOTORS_Y + by, STATUS_MOTORS_X + bx + sm, STATUS_MOTORS_Y + by + (uint8_t) (back * 12), DRAW_FILLED, VALUE_ON, OVERLAY_OR);
		
 	//Flush
	glcd_write_buffer_bounds(STATUS_MOTORS_X, STATUS_MOTORS_Y, STATUS_MOTORS_X + 32, STATUS_MOTORS_Y + 32);
}

void status_set_pid_values(uint8_t col, uint8_t row, vector_t pid_p, vector_t pid_i, vector_t pid_d){
	double value = 0;
	for (uint8_t i = 0; i < 3; i++){	//Col
		for (uint8_t j = 0; j < 2; j++){	//Row
			if (i == 0 && j == 0) value = pid_p.x;
			if (i == 1 && j == 0) value = pid_i.x;
			if (i == 2 && j == 0) value = pid_d.x;
			if (i == 0 && j == 1) value = pid_p.y;
			if (i == 1 && j == 1) value = pid_i.y;
			if (i == 2 && j == 1) value = pid_d.y;
			
			sprintf(temp, (i == 1 ? "%1.4f" : "%1.3f"), value);
			
			//Either clear the last value, or highlight the selected value
			draw_rectangle(
					STATUS_CONFIG_MODE_TEXT_X + (i + 1) * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 1) * STATUS_CONFIG_MODE_ROW_HEIGHT, 
					STATUS_CONFIG_MODE_TEXT_X + (i + 2) * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 2) * STATUS_CONFIG_MODE_ROW_HEIGHT, 
					DRAW_FILLED,
					VALUE_ON, 
					(col == i && row == j ? OVERLAY_OR : OVERLAY_NAND)
			);
			
			//Show values
			draw_text(
					STATUS_CONFIG_MODE_TEXT_X + (i + 1) * STATUS_CONFIG_MODE_COLUMN_WIDTH + 1, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 1) * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, 
					temp, 
					FONT_XSMALL_WIDTH, 
					FONT_XSMALL_HEIGHT, 
					ORIENTATION_NORMAL,
					font_xsmall, 
					codepage_ascii_caps, 
					VALUE_ON, 
					OVERLAY_XOR
			);
		}
	}

	//Flush	
	glcd_write_buffer_bounds(
			STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
			STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT, 
			STATUS_CONFIG_MODE_TEXT_X + 4 * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
			STATUS_CONFIG_MODE_TEXT_Y + 3 * STATUS_CONFIG_MODE_ROW_HEIGHT
	);
}

void status_set_kalman_values(uint8_t col, uint8_t row, vector_t kalman_qa, vector_t kalman_qg, vector_t kalman_ra){
	double value = 0;
	for (uint8_t i = 0; i < 3; i++){
		for (uint8_t j = 0; j < 2; j++){
			if (i == 0 && j == 0) value = kalman_qa.x;
			if (i == 1 && j == 0) value = kalman_qg.x;
			if (i == 2 && j == 0) value = kalman_ra.x;
			if (i == 0 && j == 1) value = kalman_qa.y;
			if (i == 1 && j == 1) value = kalman_qg.y;
			if (i == 2 && j == 1) value = kalman_ra.y;
			
			sprintf(temp, (i == 2 ? "%1.2f" : "%1.3f"), value);
			
			//Either clear the last value, or highlight the selected value
			draw_rectangle(
					STATUS_CONFIG_MODE_TEXT_X + (i + 1) * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 1) * STATUS_CONFIG_MODE_ROW_HEIGHT, 
					STATUS_CONFIG_MODE_TEXT_X + (i + 2) * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 2) * STATUS_CONFIG_MODE_ROW_HEIGHT, 
					DRAW_FILLED,
					VALUE_ON, 
					(col == i && row == j ? OVERLAY_OR : OVERLAY_NAND)
			);
			
			//Show values
			draw_text(
					STATUS_CONFIG_MODE_TEXT_X + (i + 1) * STATUS_CONFIG_MODE_COLUMN_WIDTH + 1, 
					STATUS_CONFIG_MODE_TEXT_Y + (j + 1) * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, 
					temp, 
					FONT_XSMALL_WIDTH, 
					FONT_XSMALL_HEIGHT,
					ORIENTATION_NORMAL, 
					font_xsmall, 
					codepage_ascii_caps, 
					VALUE_ON, 
					OVERLAY_XOR
			);
		}
	}

	//Flush	
	glcd_write_buffer_bounds(
			STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
			STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT, 
			STATUS_CONFIG_MODE_TEXT_X + 4 * STATUS_CONFIG_MODE_COLUMN_WIDTH, 
			STATUS_CONFIG_MODE_TEXT_Y + 3 * STATUS_CONFIG_MODE_ROW_HEIGHT
	);
}

void status_set_version_values(char* pilot_version, char* control_version){
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Pilot:", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 0 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 2 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, (char*) "Cntrl:", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	

	draw_text(STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 1 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, pilot_version, FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);
	draw_text(STATUS_CONFIG_MODE_TEXT_X + 1 * STATUS_CONFIG_MODE_COLUMN_WIDTH, STATUS_CONFIG_MODE_TEXT_Y + 2 * STATUS_CONFIG_MODE_ROW_HEIGHT + 1, control_version, FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, VALUE_ON, OVERLAY_OR);	

	glcd_write_buffer();
}

void status_persist_values(uint8_t invert){
	glcd_set_display_inverted(invert);
}