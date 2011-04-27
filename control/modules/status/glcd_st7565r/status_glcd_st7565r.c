#include "../status.h"

#include "../../../lib/glcd/glcd.h"
#include "../../../lib/glcd/draw.h"
#include "../../../lib/glcd/glcd_st7565r.h"
#include "../../../lib/glcd/fonts/xsmall.h"
#include "../../../lib/glcd/fonts/small.h"
#include "../../../lib/glcd/fonts/medium.h"
#include "../../../lib/glcd/fonts/large.h"
#include "../../../lib/glcd/fonts/xlarge.h"

/*
 * Private function, clears all data and writes labels.
 */
void _status_reset(){
	//Clear buffer
	glcd_draw_rectangle(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, DRAW_FILLED, OVERLAY_NAND);
	
	//Write text labels
	glcd_draw_text(1, 1, "Motors", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, font_xsmall, codepage_xsmall, OVERLAY_OR);
	
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

}

void status_set_telemetry(double pitch, double roll){

}

void status_set_motors(double front, double right, double back, double left){

}

void status_set_armed(uint8_t armed){

}
