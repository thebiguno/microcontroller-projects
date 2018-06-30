#include "Draw.h"

using namespace digitalcave;

void Draw::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	this->setColor(r,g,b);
	this->setAlpha(a);
}
void Draw::setColor(uint8_t r, uint8_t g, uint8_t b) {
	this->red = r;
	this->green = g;
	this->blue = b;
}
void Draw::setColor(Rgb *rgb) {
	this->setColor(rgb->getRed(), rgb->getGreen(), rgb->getBlue());
}
void Draw::setAlpha(uint8_t a) {
	this->alpha = a;
}

void Draw::setOverlay(uint8_t o) {
	overlay = o;
}
uint8_t Draw::getOverlay() {
	return overlay;
}

void Draw::translate(int8_t x, int8_t y) {
    this->translate_x = x;
    this->translate_y = y;
}

// Implementation of Bresenham's algorithm; adapted from Lady Ada's GLCD library,
// which was in turn adapted from Wikpedia.
void Draw::line(int16_t x0, int16_t y0, int16_t x1, int16_t y1){
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			setPixel(y0, x0);
		}
		else {
			setPixel(x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void Draw::rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t f) {
	//Make sure that x0,y0 is top left corner.
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);

	for(int16_t x = x0; x <= x1; x++){
		for (int16_t y = y0; y <= y1; y++){
			if (f || x == x0 || x == x1 || y == y0 || y == y1) setPixel(x, y);
		}
	}
}

//Implementation of Bresenham Algorithm for a full circle, adapted from Wikipedia sample
void Draw::circle(int16_t x0, int16_t y0, uint8_t r, uint8_t fill){
	int8_t f = 1 - r;
	int8_t ddF_x = 1;
	int8_t ddF_y = -2 * r;
	int8_t x = 0;
	int8_t y = r;

	if (fill){
		line(max(0, x0), max(0, y0 - r), max(0, x0), max(0, y0 + r));
		line(max(0, x0 - r), max(0, y0), max(0, x0 + r), max(0, y0));
	}
	else {
		setPixel(x0, y0 + r);
		setPixel(x0, y0 - r);
		setPixel(x0 + r, y0);
		setPixel(x0 - r, y0);
	}

	while(x < y) {
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - r*r + 2*x - y + 1;
		if(f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (fill){
			line(max(0, x0 - x), max(0, y0 + y), max(0, x0 + x), max(0, y0 + y));
			line(max(0, x0 - x), max(0, y0 - y), max(0, x0 + x), max(0, y0 - y));
			line(max(0, x0 - y), max(0, y0 + x), max(0, x0 + y), max(0, y0 + x));
			line(max(0, x0 - y), max(0, y0 - x), max(0, x0 + y), max(0, y0 - x));
		}
		else {
			setPixel(x0 + x, y0 + y);
			setPixel(x0 - x, y0 + y);
			setPixel(x0 + x, y0 - y);
			setPixel(x0 - x, y0 - y);
			setPixel(x0 + y, y0 + x);
			setPixel(x0 - y, y0 + x);
			setPixel(x0 + y, y0 - x);
			setPixel(x0 - y, y0 - x);
		}
	}

}
