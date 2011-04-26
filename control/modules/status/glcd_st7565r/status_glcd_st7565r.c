#include "../status.h"

#include "../../../lib/glcd/glcd.h"
#include "../../../lib/glcd/draw.h"
#include "../../../lib/glcd/glcd_st7565r.h"
#include "../../../lib/glcd/fonts/tiny.h"
#include "../../../lib/glcd/fonts/small.h"
#include "../../../lib/glcd/fonts/medium.h"
#include "../../../lib/glcd/fonts/large.h"
#include "../../../lib/glcd/fonts/xlarge.h"

void status_init(){
	st7565r_init(&PORT_LCD_DATA, PIN_LCD_DATA, 
			&PORT_LCD_CLOCK, PIN_LCD_CLOCK, 
			&PORT_LCD_A0, PIN_LCD_A0, 
			&PORT_LCD_RESET, PIN_LCD_RESET);
}


