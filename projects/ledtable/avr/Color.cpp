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
	this->h = c.h;
	this->s = c.s;
	this->v = c.v;
}

void Color::setHue(uint16_t h) {
	this->h = h % 360;
}
uint16_t Color::getHue() {
	return this->h;
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
	return this->s;
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
	return this->v;
}
void Color::addValue(int8_t v) {
	this->v += v;
	if (this->v > 100) this->v = 100;
}

void Color::complementary() {
	this->addHue(180);
}
void Color::triad() {
	this->addHue(120);
}
void Color::analagous_a() {
	this->addHue(30);
}
void Color::analagous_b() {
	this->addHue(-30);
}
void Color::split_a() {
	this->addHue(150);
}
void Color::split_b() {
	this->addHue(-150);
}
void Color::square() {
	this->addHue(90);
}

pixel_t Color::rgb() {
	float h = this->h;
	uint8_t s = this->s;
	uint8_t v = this->v;
	
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