#ifndef GLCD_H
#define GLCD_H

//Overlay modes.
#define OVERLAY_NAND	0		//Always remove pixels
#define OVERLAY_OR 		1		//Always draw pixels
#define OVERLAY_XOR		2		//XOR new pixels with buffer pixels

#define DRAW_UNFILLED	0		//Draw unfilled shapes
#define DRAW_FILLED		1		//Draw filled shapes

//The LCD width / height in pixels.  This may be used by driver implementations.
#define LCD_WIDTH 128
#define LCD_HEIGHT 32

#include <avr/io.h>

/*************
 * IMPORTANT *
 *************
 *
 * You must include a driver, which includes the following API functions.
 */
 
/* 
 * Writes the buffer to the LCD.  Only applicable for hardware implementations in which
 * the AVR buffers the display RAM locally.  For hardware implementations on which the
 * display RAM is directly accessed, this can be an empty function.
 */
void glcd_write_buffer();

/* 
 * Writes the buffer to the LCD for the given bounding box.  Only applicable for hardware 
 * implementations in which the AVR buffers the display RAM locally.  For hardware 
 * implementations on which the display RAM is directly accessed, this can be an empty function.
 */
void glcd_write_buffer_bounds(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

/*
 * Most basic operation; sets a pixel at the given X, Y co-ordinates.  Existing 
 * data will be 
 */
void glcd_set_pixel(uint8_t x, uint8_t y, uint8_t o);

/*
 * Get a a pixel at the given X, Y co-ordinates, using the specified overlay mode.
 */
uint8_t glcd_get_pixel(uint8_t x, uint8_t y);

/*
 * Optional operation; on LCDs that support it, this command will invert the display.
 * For hardware that does not support this, implementors MAY add this feature in software,
 * or leave it as a nop.
 */
void glcd_invert_display();

/*
 * Optional operation; on LCDs that support software contrast, this command will
 * adjust contrast.  For hardware that does not support this, implementors SHOULD
 * leave it as a nop.
 */
void glcd_set_contrast(uint8_t contrast);

/*
 * Finished API methods
 */

#endif