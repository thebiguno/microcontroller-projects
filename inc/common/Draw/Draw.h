#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>
#include <stdint.h>
#include <Rgb.h>

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
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;

	protected:
		uint8_t overlay = DRAW_OVERLAY_REPLACE;
		int8_t translate_x = 0;
		int8_t translate_y = 0;

	public:
		virtual void setPixel(int16_t x, int16_t y);
		virtual void flush();

		void setOverlay(uint8_t o);
		uint8_t getOverlay();

		/*
		 * Sets the color and alpha for draw operations.
		 * Not all devices are required to support color.
		 * Device frame-buffers are not required to store 8-bits per channel
		 * Typical frame-buffers are
		 * - 16-bit 4-4-4-bit 0000RRRRGGGGBBBB for 4096 colors
		 * - 16-bit 5-5-5-bit 0RRRRRGGGGGBBBBB for 32768 colors
		 * - 16-bit 5-6-5-bit RRRRRGGGGGGBBBBB for 65536 colors
		 * - 8-bit 2-2-2-bit 00RRGGBB for 64 colors
		 * - 8-bit 3-3-2-bit RRRGGGBB for 256 colors
		 * - 4-bit 3-bpp 0RGB for 8 colors
		 * Alpha information is not stored in the frame-buffer, but rather used
		 * when drawing onto the frame-buffer to determine how to alter any existing
		 * value in the buffer.  Implementations are not required to support alpha.
		 */
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		void setColor(uint8_t r, uint8_t g, uint8_t b);
		void setColor(Rgb *rgb);
		void setAlpha(uint8_t a);

		void translate(int8_t x, int8_t y);

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
