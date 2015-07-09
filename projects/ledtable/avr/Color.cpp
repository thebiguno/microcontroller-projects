#include <Math.h>
#include "Color.h"

using namespace digitalcave;

Color::Color(uint16_t h) {
	this->h = h %= 360;
	this->s = 100;
	this->v = 100;
}

Color::Color(uint16_t h, uint8_t s, uint8_t v) {
	this->h = h %= 360;
	if (s > 100) s = 100;
	this->s = s;
	if (v > 100) v = 100;
	this->v = v;
}

Color::Color(const Color &c) {
	h = c.h;
	s = c.s;
	v = c.v;
}

Color::~Color() {
	
}

void Color::setHue(uint16_t h) {
	this->h = h % 360;
}
uint16_t Color::getHue() {
	return h;
}
void Color::addHue(int16_t h) {
	this->h += h;
	this->h %= 360;
}

void Color::setSaturation(uint8_t s) {
	if (s > 100) s = 100;
	this->s = s;
}
uint8_t Color::getSaturation() {
	return s;
}
void Color::addSaturation(int8_t s) {
	this->s += s;
	if (this->s > 100) this->s = 100;
}

void Color::setValue(uint8_t v) {
	if (v > 100) v = 100;
	this->v = v;
}
uint8_t Color::getValue() {
	return v;
}
void Color::addValue(int8_t v) {
	this->v += v;
	if (this->v > 100) this->v = 100;
}

void Color::complementary() {
	addHue(180);
}
void Color::triad() {
	addHue(120);
}
void Color::analagous_a() {
	addHue(30);
}
void Color::analagous_b() {
	addHue(-30);
}
void Color::split_a() {
	addHue(150);
}
void Color::split_b() {
	addHue(-150);
}
void Color::square() {
	addHue(90);
}

pixel_t Color::rgb() {
	pixel_t rgb;
	uint8_t i = floor(h / 60); // sector 0 to 5
	float f = h - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	
	switch(i) {
		case 0:
		rgb.red = v;
		rgb.green = t;
		rgb.blue = p;
		break;
		case 1:
		rgb.red = q;
		rgb.green = v;
		rgb.blue = p;
		break;
		case 2:
		rgb.red = p;
		rgb.green = v;
		rgb.blue = t;
		break;
		case 3:
		rgb.red = p;
		rgb.green = q;
		rgb.blue = v;
		break;
		case 4:
		rgb.red = t;
		rgb.green = p;
		rgb.blue = v;
		break;
		default:
		rgb.red = v;
		rgb.green = p;
		rgb.blue = q;
		break;
	}
	return rgb;
}