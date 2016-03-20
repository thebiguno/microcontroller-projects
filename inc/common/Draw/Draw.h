#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>
#include <stdint.h> 

//Orientation modes.
#define DRAW_ORIENTATION_NORMAL	0		//Normal, left to right
#define DRAW_ORIENTATION_DOWN	1		//Rotated 90 degrees CW
#define DRAW_ORIENTATION_0		0		//Normal, left to right
#define DRAW_ORIENTATION_90		1		//Rotated 90 degrees CW
#define DRAW_ORIENTATION_180	2		//Rotated 180 degrees CW
#define DRAW_ORIENTATION_270	3		//Rotated 270 degrees CW

//#define DRAW_ORIENTATION_UP		2		//Rotated 90 degrees CCW (i.e. 270 degrees CW)

//Overlay modes.
#define DRAW_OVERLAY_REPLACE	0		//Replace value
#define DRAW_OVERLAY_NAND		1		//Always remove pixels
#define DRAW_OVERLAY_OR 		2		//Always draw pixels
#define DRAW_OVERLAY_XOR		3		//XOR new pixels with buffer pixels

//Fill modes.
#define DRAW_UNFILLED			0		//Draw unfilled shapes
#define DRAW_FILLED				1		//Draw filled shapes

#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define max(a, b) (a > b ? a : b)


namespace digitalcave {
	class Draw {
	private:
		uint8_t font_glyph_byte_ct;
		uint8_t font_width;
		uint8_t font_height;
		uint8_t* font;
		uint8_t* font_codepage;

	protected:
		uint8_t overlay = DRAW_OVERLAY_REPLACE;
		
	public:	
		virtual void setPixel(int16_t x, int16_t y);
		virtual void flush();
		
		void setOverlay(uint8_t o);
		uint8_t getOverlay();
		
		/*
		 * Draws a bitmap from flash memory of the specified size on the screen at the specified position.
		 * For ever bit set in the bitmap a pixel of the specified value will be drawn with the specified overlay.
		 */
		void bitmap(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t orientation, uint8_t* bitmap);
		
		void setFont(uint8_t* font, uint8_t* codepage, uint8_t width, uint8_t height);
		/*
		 * Draws text on screen at the specified position, using the given font, in the given overlay mode.
		 */
		void text(int16_t x, int16_t y, const char* text, uint8_t orientation);
		void character(int16_t x, int16_t y, char c, uint8_t orientation);
		
		/*
		 * Draws a line between two points x0,y0 to x1,y1.  Overlay (o) determines the
		 * overlay mode to use.
		 */
		void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

		/*
		 * Draws a box with one corner at x0, y0 and the opposite corner at x1, y1.
		 * If fill is 0, it is just an outline; if f is non-zero it is filled.
		 * Value determines the color (driver-specific meaning).
		 * Overlay determines the overlay method to use.
		 */
		void rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t fill);

		/*
		 * Draws a circle of radius r centered at point x, y.  If f (fill) is 0, it is just
		 * the outline; if f is non-zero, then the circle is filled.  Overlay (o) determines
		 * the overlay method to use.
		 */
		 void circle(int16_t x, int16_t y, uint8_t r, uint8_t fill);
	};		
}

#endif