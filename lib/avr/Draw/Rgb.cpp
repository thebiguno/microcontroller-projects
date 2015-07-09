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
	uint8_t s = c.getSaturation();
	uint8_t v = c.getValue();
	if (s == 0) {
		// achromatic (grey)
		r = g = b = v;
	} else {
		float h = ((float) c.getHue()) / 60; // sector 0 to 5
	
		uint8_t i = floor( h );
		float f = h - i;			// factorial part of h
		float p = v * ( 1 - s );
		float q = v * ( 1 - s * f );
		float t = v * ( 1 - s * ( 1 - f ) );
		
		switch(i) {
			case 0: r = v; g = t; v = p; break;
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