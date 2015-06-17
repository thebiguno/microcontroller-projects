#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

//Orientation modes.
#define ORIENTATION_NORMAL	0		//Normal, left to right
#define ORIENTATION_DOWN	1		//Rotated 90 degrees CW
//#define ORIENTATION_UP		2		//Rotated 90 degrees CCW (i.e. 270 degrees CW)

//Overlay modes.
#define OVERLAY_REPLACE	0		//Replace value
#define OVERLAY_NAND	1		//Always remove pixels
#define OVERLAY_OR 		2		//Always draw pixels
#define OVERLAY_XOR		3		//XOR new pixels with buffer pixels

//Fill modes.
#define DRAW_UNFILLED	0		//Draw unfilled shapes
#define DRAW_FILLED		1		//Draw filled shapes

/*
 * This must be implemented by the driver.
 */
void set_pixel(int16_t x, int16_t y, pixel_t value, uint8_t overlay);

/* 
 * This must be implemented by the driver.
 */
pixel_t get_pixel(uint8_t x, uint8_t y);

/*
 * Gets raw values of the specified size from the canvas at the specified location.
 */
void draw_set_raw(int16_t x, int16_t y, uint8_t width, uint8_t height, pixel_t* raw, uint8_t overlay);

/*
 * Sets raw values of the specified size to the canvas at the specified location.
 * The raw array must be large enough to hold width * height values; no bounds checking is done.
 */
void draw_get_raw(uint8_t x, uint8_t y, uint8_t width, uint8_t height, pixel_t* raw);

/*
 * Draws a bitmap from flash memory of the specified size on the screen at the specified position.
 * For ever bit set in the bitmap a pixel of the specified value will be drawn with the specified overlay.
 */
void draw_bitmap(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* bitmap, pixel_t value, uint8_t overlay);

/*
 * Draws text on screen at the specified position, using the given font, in
 * the given overlay mode.
 * The Width / Height are for the glyphs in the given font.
 */
void draw_text(int16_t x, int16_t y, char* text, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* font, uint8_t* codepage, pixel_t value, uint8_t overlay);

/*
 * Draws a line between two points x0,y0 to x1,y1.  Overlay (o) determines the
 * overlay mode to use.
 */
void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, pixel_t value, uint8_t overlay);

/*
 * Draws a box with one corner at x0, y0 and the opposite corner at x1, y1.
 * If fill is 0, it is just an outline; if f is non-zero it is filled.
 * Value determines the color (driver-specific meaning).
 * Overlay determines the overlay method to use.
 */
void draw_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t fill, pixel_t value, uint8_t overlay);

/*
 * Draws a circle of radius r centered at point x, y.  If f (fill) is 0, it is just
 * the outline; if f is non-zero, then the circle is filled.  Overlay (o) determines
 * the overlay method to use.
 */
 void draw_circle(int16_t x, int16_t y, uint8_t r, uint8_t fill, pixel_t value, uint8_t overlay);
 

/********** Helper defines *************/

#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define max(a, b) (a > b ? a : b)

#endif
