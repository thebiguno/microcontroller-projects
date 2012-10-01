#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>
#include <avr/pgmspace.h> 

//Orientation modes.
#define ORIENTATION_NORMAL	0		//Normal, left to right
#define ORIENTATION_DOWN	1		//Rotated 90 degrees CW
//#define ORIENTATION_UP		2		//Rotated 90 degrees CCW (i.e. 270 degrees CW)

//Overlay modes.
#define OVERLAY_NAND	0		//Always remove pixels
#define OVERLAY_OR 		1		//Always draw pixels
#define OVERLAY_XOR		2		//XOR new pixels with buffer pixels

#define DRAW_UNFILLED	0		//Draw unfilled shapes
#define DRAW_FILLED		1		//Draw filled shapes

/*
 * The core of all draw implementations; this must be implemented by the driver.
 */
void set_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay);

/* 
 * This must be implemented by the driver.
 */
uint8_t get_pixel(uint8_t x, uint8_t y);

/*
 * Get a a pixel at the given X, Y co-ordinates.  This must also be implemented by the driver.  The meaning of the return value
 * depends on the driver implementation, although in general it SHOULD be zero for off and non zero for on (in various 
 * colors, etc)
 */
uint8_t matrix_get_pixel(uint8_t x, uint8_t y);



/*
 * Draws text on screen at the specified position, using the given font, in
 * the given overlay mode.
 * The Width / Height are for the glyphs in the given font.
 */
void draw_text(uint8_t x, uint8_t y, char* text, uint8_t width, uint8_t height, uint8_t orientation, prog_uchar* font, prog_uchar* codepage, uint8_t value, uint8_t overlay);

/*
 * Draws a bitmap on the screen at the specified position, of specified width / height,
 * in the specified overlay mode.
 */
void draw_bitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t orientation, prog_uchar* bitmap,  uint8_t value,uint8_t overlay);

/*
 * Draws a line between two points x0,y0 to x1,y1.  Overlay (o) determines the
 * overlay mode to use.
 */
void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t value, uint8_t overlay);

/*
 * Draws a box with one corner at x0, y0 and the opposite corner at x1, y1.
 * If fill is 0, it is just an outline; if f is non-zero it is filled.
 * Value determines the color (driver-specific meaning).
 * Overlay determines the overlay method to use.
 */
void draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t fill, uint8_t value, uint8_t overlay);

/*
 * Draws a circle of radius r centered at point x, y.  If f (fill) is 0, it is just
 * the outline; if f is non-zero, then the circle is filled.  Overlay (o) determines
 * the overlay method to use.
 */
 void draw_circle(uint8_t x, uint8_t y, uint8_t r, uint8_t fill, uint8_t value, uint8_t overlay);
 

/********** Helper defines *************/

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#endif