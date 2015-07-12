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
	uint16_t h = c.getHue();
	if (s == 0) {
		// achromatic (grey)
		r = g = b = v;
	} else {
		uint8_t base = ((255 - s) * v) >> 8;
		
		switch(h / 60) {
			case 0: r = v; g = (((v-base)*h)/60)+base; b = base; break;
			case 1: r = (((v-base)*(60-(h%60)))/60)+base; g = v; b = base; break;
			case 2: r = base; g = v; b = (((v-base)*(h%60))/60)+base; break;
			case 3: r = base; g = (((v-base)*(60-(h%60)))/60)+base; b = v; break;
			case 4: r = (((v-base)*(h%60))/60)+base; g = base; b = v; break;
			default: r = v; g = base; b = (((v-base)*(60-(h%60)))/60)+base; break;
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