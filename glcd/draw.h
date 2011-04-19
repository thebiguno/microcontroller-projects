#ifndef GLCD_DRAW_H
#define GLCD_DRAW_H

#include "glcd.h"
#include <stdlib.h>
#include <avr/pgmspace.h> 


/*
 * Draws text on screen at the specified position, using the given font, in
 * the given overlay mode.
 * The Width / Height are for the glyphs in the given font.
 */
void glcd_draw_text(uint8_t x, uint8_t y, char* text, uint8_t width, uint8_t height, prog_uchar* font, prog_uchar* codepage, uint8_t o);

/*
 * Draws a bitmap on the screen at the specified position, of specified width / height,
 * in the specified overlay mode.
 */
void glcd_draw_bitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t height, prog_uchar* bitmap, uint8_t o);

/*
 * Draws a line between two points x0,y0 to x1,y1.  Overlay (o) determines the
 * overlay mode to use.
 */
void glcd_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t o);

/*
 * Draws a box with one corner at x0, y0 and the opposite corner at x1, y1.
 * If f (fill) is 0, it is just an outline; if f is non-zero it is filled.
 * Overlay (o) determines the overlay method to use.
 */
void glcd_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t f, uint8_t o);

/*
 * Draws a circle of radius r centered at point x, y.  If f (fill) is 0, it is just
 * the outline; if f is non-zero, then the circle is filled.  Overlay (o) determines
 * the overlay method to use.
 */
 void glcd_draw_circle(uint8_t x, uint8_t y, uint8_t r, uint8_t f, uint8_t o);
 

/********** Helper defines *************/

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#endif