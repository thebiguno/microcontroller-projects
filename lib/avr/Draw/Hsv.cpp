#include "Hsv.h"

using namespace digitalcave;

Hsv::Hsv(uint16_t h) {
	this->h = h %= 360;
	this->s = 255;
	this->v = 255;
}

Hsv::Hsv(uint16_t h, uint8_t s, uint8_t v) {
	this->h = h %= 360;
	this->s = s;
	this->v = v;
}

Hsv::Hsv(const Hsv &c) {
	h = c.h;
	s = c.s;
	v = c.v;
}

Hsv::~Hsv() {
	
}

void Hsv::setHue(uint16_t h) {
	this->h = h % 360;
}
uint16_t Hsv::getHue() const {
	return h;
}
void Hsv::addHue(int16_t h) {
	this->h += h;
	this->h %= 360;
}

void Hsv::setSaturation(uint8_t s) {
	if (s > 100) s = 100;
	this->s = s;
}
uint8_t Hsv::getSaturation() const {
	return s;
}
void Hsv::addSaturation(int8_t s) {
	this->s += s;
}

void Hsv::setValue(uint8_t v) {
	if (v > 100) v = 100;
	this->v = v;
}
uint8_t Hsv::getValue() const {
	return v;
}
void Hsv::addValue(int8_t v) {
	this->v += v;
}