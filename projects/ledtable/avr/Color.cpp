#include "Color.h"
#include "lib/draw/ws2812.h"

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

Color::Color(Color c) {
	this->h = c->h;
	this->s = c->s;
	this->v = c->v;
}

void Color::complementary() {
	this->h = (c + 180) % 360;
}
void Color::triad() {
	this->h = (c + 120) % 360;
}
void Color::analagous_a(uint16_t c) {
	return (c + 30) % 360;
}
uint16_t Color::analagous_b(uint16_t c) {
	return (c + 330) % 360;
}
uint16_t Color::split_a(uint16_t c) {
	return (c + 150) % 360;
}
uint16_t Color:split_b(uint16_t c) {
	return (c + 210) % 360;
}
uint16_t Color::square(uint16_t c) {
	return (c + 90) % 360;
}

void Color::setHue(uint16_t h) {
	this->h = h % 360;
}
uint16_t Color::getHue() {
	return this->h;
}

void Color::setSaturation(uint8_t s) {
	if (s > 100) s = 100;
	this->s = s;
}
uint8_t Color::getHue() {
	return this->s;
}

void Color::setValue(uint8_t v) {
	if (v > 100) v = 100;
	this->v = v;
}
uint8_t Color::getValue() {
	return this->v;
}

pixel_t Color::h2rgb() {
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
		rgb->red = v;
		rgb->green = t;
		rgb->blue = p;
		break;
		case 1:
		rgb->red = q;
		rgb->green = v;
		rgb->blue = p;
		break;
		case 2:
		rgb->red = p;
		rgb->green = v;
		rgb->blue = t;
		break;
		case 3:
		rgb->red = p;
		rgb->green = q;
		rgb->blue = v;
		break;
		case 4:
		rgb->red = t;
		rgb->green = p;
		rgb->blue = v;
		break;
		default:
		rgb->red = v;
		rgb->green = p;
		rgb->blue = q;
		break;
	}
}