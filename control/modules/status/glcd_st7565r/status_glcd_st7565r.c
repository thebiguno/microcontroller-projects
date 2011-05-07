#include <stdio.h>

#include "../status.h"

#include "../../../lib/glcd/glcd.h"
#include "../../../lib/glcd/draw.h"
#include "../../../lib/glcd/glcd_st7565r.h"
#include "../../../lib/glcd/fonts/ascii.h"
#include "../../../lib/glcd/fonts/xsmall.h"
#include "../../../lib/glcd/fonts/small.h"
#include "../../../lib/glcd/fonts/medium.h"
#include "../../../lib/glcd/fonts/large.h"
#include "../../../lib/glcd/fonts/xlarge.h"

#define STATUS_MOTORS_X				0
#define STATUS_MOTORS_Y				0
#define STATUS_THROTTLE_X			30
#define STATUS_THROTTLE_Y			0
#define STATUS_PITCH_X				75
#define STATUS_PITCH_Y				0
#define STATUS_ROLL_X				100
#define STATUS_ROLL_Y				0
#define STATUS_TIME_X				30
#define STATUS_TIME_Y				18
#define STATUS_BATTERY_PILOT_X		105
#define STATUS_BATTERY_PILOT_Y		22
#define STATUS_BATTERY_CONTROL_X	75
#define STATUS_BATTERY_CONTROL_Y	22
#define STATUS_COMM_X				75
#define STATUS_COMM_Y				15

//Temp buffer for printf
char temp[16];

/*
 * Private function, clears all data and writes labels.
 */
void _status_reset(){
	//Clear buffer
	glcd_draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, OVERLAY_NAND);
	
	//Write text labels
	glcd_draw_text(STATUS_MOTORS_X, STATUS_MOTORS_Y, "Motors", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	glcd_draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, "Throttle", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	glcd_draw_text(STATUS_PITCH_X, STATUS_PITCH_Y, "Pitch", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	glcd_draw_text(STATUS_TIME_X, STATUS_TIME_Y, "Armed Time", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	glcd_draw_text(STATUS_ROLL_X, STATUS_ROLL_Y, "Roll", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	
	glcd_write_buffer();
}

void status_init(){
	st7565r_init(&PORT_LCD_DATA, PIN_LCD_DATA, 
			&PORT_LCD_CLOCK, PIN_LCD_CLOCK, 
			&PORT_LCD_A0, PIN_LCD_A0, 
			&PORT_LCD_RESET, PIN_LCD_RESET);
			
	_status_reset();
}


void status_set_battery_level(double value, uint8_t x, uint8_t y, char* name){
	//Clear existing
	glcd_draw_rectangle(x, y, x + 21, y + 9, DRAW_FILLED, OVERLAY_NAND);
	
	//Redraw edge
	glcd_draw_rectangle(x, y, x + 20, y + 8, DRAW_UNFILLED, OVERLAY_OR);
	glcd_draw_line(x + 21, y + 2, x + 21, y + 7, OVERLAY_OR);
	
	//Fill according to level
	glcd_draw_rectangle(x + 1, y + 1, x + 1 + (int) (value * 20), y + 7, DRAW_FILLED, OVERLAY_OR);
	
	//Print name
	glcd_draw_text(x + 2, y + 2, name, FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_XOR);
	
	//Flush
	glcd_write_buffer_bounds(x, y, x + 21, y + 9);
}

void status_set_pilot_battery_level(double value){
	status_set_battery_level(value, STATUS_BATTERY_PILOT_X, STATUS_BATTERY_PILOT_Y, "P");
}

void status_set_control_battery_level(double value){
	status_set_battery_level(value, STATUS_BATTERY_CONTROL_X, STATUS_BATTERY_CONTROL_Y, "C");
}

void status_set_telemetry(double pitch, double roll){
	//Clear existing
	glcd_draw_rectangle(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 20, STATUS_PITCH_Y + 13, DRAW_FILLED, OVERLAY_NAND);
	glcd_draw_rectangle(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 20, STATUS_ROLL_Y + 13, DRAW_FILLED, OVERLAY_NAND);	

	//Write values
	sprintf(temp, "%3.1d", (int8_t) (pitch * 57.2957795));
	glcd_draw_text(STATUS_PITCH_X, STATUS_PITCH_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_ascii_caps, OVERLAY_OR);
	sprintf(temp, "%3.1d", (int8_t) (roll * 57.2957795));
	glcd_draw_text(STATUS_ROLL_X, STATUS_ROLL_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_ascii_caps, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 20, STATUS_PITCH_Y + 13);
	glcd_write_buffer_bounds(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 20, STATUS_ROLL_Y + 13);
}

void status_set_throttle(double throttle, uint8_t armed){
	if (armed){
		if (throttle > 0.99) throttle = 0.99;
		if (throttle < 0) throttle = 0;
	
		//Clear existing
		glcd_draw_rectangle(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13, DRAW_FILLED, OVERLAY_NAND);
	
		//Write values
		sprintf(temp, "%2.1d", (int8_t) (throttle * 100));
		glcd_draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_ascii_caps, OVERLAY_OR);
		
		//Flush
		glcd_write_buffer_bounds(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13);
	}
	else {
		//Clear existing
		glcd_draw_rectangle(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13, DRAW_FILLED, OVERLAY_NAND);
	
		glcd_draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, "---", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_ascii_caps, OVERLAY_OR);
		
		//Flush
		glcd_write_buffer_bounds(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13);
	}	

}

void status_set_motors(double left, double front, double right, double back){
	//Clear existing
	glcd_draw_rectangle(STATUS_MOTORS_X, STATUS_MOTORS_Y + 6, STATUS_MOTORS_X + 20, STATUS_MOTORS_Y + 23, DRAW_FILLED, OVERLAY_NAND);

	//Redraw edges: top, right, bottom, left
	glcd_draw_rectangle(STATUS_MOTORS_X + 8, STATUS_MOTORS_Y + 6, STATUS_MOTORS_X + 13, STATUS_MOTORS_Y + 17, DRAW_UNFILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 15, STATUS_MOTORS_Y + 12, STATUS_MOTORS_X + 20, STATUS_MOTORS_Y + 23, DRAW_UNFILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 8, STATUS_MOTORS_Y + 19, STATUS_MOTORS_X + 13, STATUS_MOTORS_Y + 30, DRAW_UNFILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 1, STATUS_MOTORS_Y + 12, STATUS_MOTORS_X + 6, STATUS_MOTORS_Y + 23, DRAW_UNFILLED, OVERLAY_OR);
	
	//Fill according to motor levels
	glcd_draw_rectangle(STATUS_MOTORS_X + 8, STATUS_MOTORS_Y + 6 + (uint8_t) ((1.0 - front) * 9), STATUS_MOTORS_X + 13, STATUS_MOTORS_Y + 17, DRAW_FILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 15, STATUS_MOTORS_Y + 12 + (uint8_t) ((1.0 - right) * 9), STATUS_MOTORS_X + 20, STATUS_MOTORS_Y + 23, DRAW_FILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 8, STATUS_MOTORS_Y + 19 + (uint8_t) ((1.0 - back) * 9), STATUS_MOTORS_X + 13, STATUS_MOTORS_Y + 30, DRAW_FILLED, OVERLAY_OR);
	glcd_draw_rectangle(STATUS_MOTORS_X + 1, STATUS_MOTORS_Y + 12 + (uint8_t) ((1.0 - left) * 9), STATUS_MOTORS_X + 6, STATUS_MOTORS_Y + 23, DRAW_FILLED, OVERLAY_OR);
		
	//Flush
	glcd_write_buffer_bounds(STATUS_MOTORS_X, STATUS_MOTORS_Y + 6, STATUS_MOTORS_X + 20, STATUS_MOTORS_Y + 24);
}

void status_set_armed_time(uint32_t millis){
	//Clear existing
	glcd_draw_rectangle(STATUS_TIME_X, STATUS_TIME_Y + 6, STATUS_TIME_X + 30, STATUS_TIME_Y + 13, DRAW_FILLED, OVERLAY_NAND);

	//Write values
	sprintf(temp, "%02d:%02d", (uint8_t) (millis / 60000), (uint8_t) ((millis % 60000) / 1000));
	glcd_draw_text(STATUS_TIME_X, STATUS_TIME_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_ascii_caps, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_TIME_X, STATUS_TIME_Y + 6, STATUS_TIME_X + 30, STATUS_TIME_Y + 13);
}

void status_set_comm_state(uint8_t tx_pilot, uint8_t rx_pilot, uint8_t tx_pc, uint8_t rx_pc){
	//Clear existing
	glcd_draw_rectangle(STATUS_COMM_X, STATUS_COMM_Y, STATUS_COMM_X + 36, STATUS_COMM_Y + 5, DRAW_FILLED, OVERLAY_NAND);

	//Write values
	if (tx_pilot) glcd_draw_text(STATUS_COMM_X, STATUS_COMM_Y, "TX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	if (rx_pilot) glcd_draw_text(STATUS_COMM_X + 9, STATUS_COMM_Y, "RX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	if (tx_pc) glcd_draw_text(STATUS_COMM_X + 18, STATUS_COMM_Y, "TX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	if (rx_pc) glcd_draw_text(STATUS_COMM_X + 27, STATUS_COMM_Y, "RX", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_ascii_caps, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_COMM_X, STATUS_COMM_Y, STATUS_COMM_X + 36, STATUS_COMM_Y + 5);

}
