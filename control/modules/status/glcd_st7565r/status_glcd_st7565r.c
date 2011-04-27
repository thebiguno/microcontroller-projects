#include <stdio.h>

#include "../status.h"

#include "../../../lib/glcd/glcd.h"
#include "../../../lib/glcd/draw.h"
#include "../../../lib/glcd/glcd_st7565r.h"
#include "../../../lib/glcd/fonts/xsmall.h"
#include "../../../lib/glcd/fonts/small.h"
#include "../../../lib/glcd/fonts/medium.h"
#include "../../../lib/glcd/fonts/large.h"
#include "../../../lib/glcd/fonts/xlarge.h"

#define STATUS_MOTORS_X		0
#define STATUS_MOTORS_Y		0
#define STATUS_THROTTLE_X	40
#define STATUS_THROTTLE_Y	0
#define STATUS_PITCH_X		75
#define STATUS_PITCH_Y		0
#define STATUS_ROLL_X		75
#define STATUS_ROLL_Y		16
#define STATUS_TIME_X		30
#define STATUS_TIME_Y		16
#define STATUS_BATTERY_X	105
#define STATUS_BATTERY_Y	20
#define STATUS_ARMED_X		97
#define STATUS_ARMED_Y		0


//Temp buffer for printf
char temp[16];
/*
prog_uchar battery_0[] PROGMEM = {0xff,0xff,0xc8,0x00,0x06,0x80,0x00,0x28,0x00,0x03,0x80,0x00,0x38,0x00,0x02,0x80,0x00,0x6f,0xff,0xfc};
prog_uchar battery_1[] PROGMEM = {0xff,0xff,0xcc,0x00,0x06,0xc0,0x00,0x2e,0x00,0x03,0xe0,0x00,0x3f,0x00,0x02,0xf0,0x00,0x6f,0xff,0xfc};
prog_uchar battery_2[] PROGMEM = {0xff,0xff,0xcf,0x00,0x06,0xf0,0x00,0x2f,0x80,0x03,0xf8,0x00,0x3f,0xc0,0x02,0xfc,0x00,0x6f,0xff,0xfc};
prog_uchar battery_3[] PROGMEM = {0xff,0xff,0xcf,0xc0,0x06,0xfc,0x00,0x2f,0xe0,0x03,0xfe,0x00,0x3f,0xf0,0x02,0xff,0x00,0x6f,0xff,0xfc};
prog_uchar battery_4[] PROGMEM = {0xff,0xff,0xcf,0xf0,0x06,0xff,0x00,0x2f,0xf8,0x03,0xff,0x80,0x3f,0xfc,0x02,0xff,0xc0,0x6f,0xff,0xfc};
prog_uchar battery_5[] PROGMEM = {0xff,0xff,0xcf,0xfc,0x06,0xff,0xc0,0x2f,0xfe,0x03,0xff,0xe0,0x3f,0xff,0x02,0xff,0xf0,0x6f,0xff,0xfc};
prog_uchar battery_6[] PROGMEM = {0xff,0xff,0xcf,0xff,0x06,0xff,0xf0,0x2f,0xff,0x83,0xff,0xf8,0x3f,0xff,0xc2,0xff,0xfc,0x6f,0xff,0xfc};
prog_uchar battery_7[] PROGMEM = {0xff,0xff,0xcf,0xff,0xc6,0xff,0xfc,0x2f,0xff,0xe3,0xff,0xfe,0x3f,0xff,0xf2,0xff,0xff,0x6f,0xff,0xfc};
prog_uchar battery_8[] PROGMEM = {0xff,0xff,0xcf,0xff,0xf6,0xff,0xff,0x2f,0xff,0xfb,0xff,0xff,0xbf,0xff,0xfe,0xff,0xff,0xef,0xff,0xfc};
prog_uchar battery_9[] PROGMEM = {0xff,0xff,0xcf,0xff,0xfe,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xef,0xff,0xfc};
*/

/*
 * Private function, clears all data and writes labels.
 */
void _status_reset(){
	//Clear buffer
	glcd_draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, OVERLAY_NAND);
	
	//Write text labels
	glcd_draw_text(STATUS_MOTORS_X, STATUS_MOTORS_Y, "Motors", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	glcd_draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y, "Throttle", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	glcd_draw_text(STATUS_PITCH_X, STATUS_PITCH_Y, "Pitch", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	glcd_draw_text(STATUS_TIME_X, STATUS_TIME_Y, "Time", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	glcd_draw_text(STATUS_ROLL_X, STATUS_ROLL_Y, "Roll", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	
	glcd_write_buffer();
}

void status_init(){
	st7565r_init(&PORT_LCD_DATA, PIN_LCD_DATA, 
			&PORT_LCD_CLOCK, PIN_LCD_CLOCK, 
			&PORT_LCD_A0, PIN_LCD_A0, 
			&PORT_LCD_RESET, PIN_LCD_RESET);
			
	_status_reset();
}

void status_set_battery_level(double value){
	//Clear existing
	glcd_draw_rectangle(STATUS_BATTERY_X, STATUS_BATTERY_Y, STATUS_BATTERY_X + 21, STATUS_BATTERY_Y + 9, DRAW_FILLED, OVERLAY_NAND);
	
	//Redraw edge
	glcd_draw_rectangle(STATUS_BATTERY_X, STATUS_BATTERY_Y, STATUS_BATTERY_X + 20, 28, DRAW_UNFILLED, OVERLAY_OR);
	glcd_draw_line(STATUS_BATTERY_X + 21, STATUS_BATTERY_Y + 2, STATUS_BATTERY_X + 21, STATUS_BATTERY_Y + 7, OVERLAY_OR);
	
	//Fill according to level
	glcd_draw_rectangle(STATUS_BATTERY_X + 1, STATUS_BATTERY_Y + 1, STATUS_BATTERY_X + 1 + (int) (value * 20), STATUS_BATTERY_Y + 7, DRAW_FILLED, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_BATTERY_X, STATUS_BATTERY_Y, STATUS_BATTERY_X + 21, STATUS_BATTERY_Y + 9);
}

void status_set_telemetry(double pitch, double roll){
	//Clear existing
	glcd_draw_rectangle(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 20, STATUS_PITCH_Y + 13, DRAW_FILLED, OVERLAY_NAND);
	glcd_draw_rectangle(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 20, STATUS_ROLL_Y + 13, DRAW_FILLED, OVERLAY_NAND);	

	//Write values
	sprintf(temp, "%3.1d", (int8_t) pitch);
	glcd_draw_text(STATUS_PITCH_X, STATUS_PITCH_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_small, OVERLAY_OR);
	sprintf(temp, "%3.1d", (int8_t) roll);
	glcd_draw_text(STATUS_ROLL_X, STATUS_ROLL_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_small, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_PITCH_X, STATUS_PITCH_Y + 6, STATUS_PITCH_X + 20, STATUS_PITCH_Y + 13);
	glcd_write_buffer_bounds(STATUS_ROLL_X, STATUS_ROLL_Y + 6, STATUS_ROLL_X + 20, STATUS_ROLL_Y + 13);
}

void status_set_throttle(double throttle){
	//Clear existing
	glcd_draw_rectangle(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13, DRAW_FILLED, OVERLAY_NAND);

	//Write values
	sprintf(temp, "%3.1d", (int8_t) throttle);
	glcd_draw_text(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_small, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_THROTTLE_X, STATUS_THROTTLE_Y + 6, STATUS_THROTTLE_X + 20, STATUS_THROTTLE_Y + 13);
}

void status_set_motors(double front, double right, double back, double left){

}

void status_set_armed(uint8_t armed){
	//Clear existing
	glcd_draw_rectangle(STATUS_ARMED_X, STATUS_ARMED_Y, STATUS_ARMED_X + 30, STATUS_ARMED_Y + 7, DRAW_FILLED, OVERLAY_NAND);

	//Write values
	if (armed){
		glcd_draw_text(STATUS_ARMED_X, STATUS_ARMED_Y, "ARMED", FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_small, OVERLAY_OR);
	}
	
	//Flush
	glcd_write_buffer_bounds(STATUS_ARMED_X, STATUS_ARMED_Y, STATUS_ARMED_X + 30, STATUS_ARMED_Y + 7);
}

void status_set_armed_time(uint32_t millis){
	//Clear existing
	glcd_draw_rectangle(STATUS_TIME_X, STATUS_TIME_Y + 6, STATUS_TIME_X + 30, STATUS_TIME_Y + 13, DRAW_FILLED, OVERLAY_NAND);

	//Write values
	sprintf(temp, "%02d:%02d", (uint8_t) (millis / 60000), (uint8_t) ((millis % 60000) / 1000));
	glcd_draw_text(STATUS_TIME_X, STATUS_TIME_Y + 6, temp, FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, font_small, codepage_small, OVERLAY_OR);
	
	//Flush
	glcd_write_buffer_bounds(STATUS_TIME_X, STATUS_TIME_Y + 6, STATUS_TIME_X + 30, STATUS_TIME_Y + 13);
}