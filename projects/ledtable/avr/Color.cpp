#include "Color.h"
#include "lib/draw/ws2812.h"

using namespace digitalcave;

uint16_t Color::complementary(uint16_t c) {
	return (c + 180) % 360;
}
uint16_t Color::triad(uint16_t c) {
	return (c + 120) % 360;
}
uint16_t Color::analagous_a(uint16_t c) {
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

void Color::h2rgb(float h, struct ws2812_t *rgb) {
	h /= 60;			// sector 0 to 5
	uint8_t i = floor(h);
	float f = h - i;	// factorial part of h
	float q = 1 - f;
	float t = 1 - (1 - f);
	switch(i) {
		case 0:
		rgb->red = 0xff;
		rgb->green = 0xff * t;
		rgb->blue = 0;
		break;
		case 1:
		rgb->red = 0xff * q;
		rgb->green = 0xff;
		rgb->blue = 0;
		break;
		case 2:
		rgb->red = 0;
		rgb->green = 0xff;
		rgb->blue = 0xff * t;
		break;
		case 3:
		rgb->red = 0;
		rgb->green = 0xff * q;
		rgb->blue = 0xff;
		break;
		case 4:
		rgb->red = 0xff * t;
		rgb->green = 0;
		rgb->blue = 0xff;
		break;
		default:
		rgb->red = 0xff;
		rgb->green = 0;
		rgb->blue = 0xff * q;
		break;
	}
}