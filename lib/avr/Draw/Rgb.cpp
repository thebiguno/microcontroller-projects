#include <Math.h>
#include "Rgb.h"

using namespace digitalcave;

Rgb::Rgb(uint8_t r, uint8_t g, uint8_t b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

Rgb::Rgb(const Rgb &c) {
	r = c.r;
	g = c.g;
	b = c.b;
}

Rgb::Rgb(const Hsv &c) {
	float s = c.getSaturation();
	float v = c.getValue();
	s /= 255.0;
	v /= 255.0;
	
	if (s == 0) {
		// achromatic (grey)
		r = g = b = v * 255;
	} else {
		float h = c.getHue();
		h /= 60;
		uint8_t i = floor(h);
		float f = h - i;

		float p = v * (1.0 - s);
		float q = v * (1.0 - (s * f));
		float t = v * (1.0 - (s * (1.0 - f)));
		
		p *= 255;
		q *= 255;
		t *= 255;
		v *= 255;
		
		switch (i) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			default: r = v; g = p; b = q; break;
		}
	}
}

Rgb::~Rgb() {
	
}

void Rgb::setRed(uint8_t r) {
	this->r = r;
}
uint8_t Rgb::getRed() const {
	return r;
}
void Rgb::addRed(int8_t r) {
	this->r += r;
}

void Rgb::setGreen(uint8_t s) {
	this->g = g;
}
uint8_t Rgb::getGreen() const {
	return g;
}
void Rgb::addGreen(int8_t g) {
	this->g += g;
}

void Rgb::setBlue(uint8_t b) {
	this->b = b;
}
uint8_t Rgb::getBlue() const {
	return b;
}
void Rgb::addBlue(int8_t b) {
	this->b += b;
}