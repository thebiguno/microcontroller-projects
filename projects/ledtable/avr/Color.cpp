#include "Color.h"
#include "lib/draw/ws2812.h"

using namespace digitalcave;

ws2812_t Color::black = {};
ws2812_t Color::white = { .red = 0xff; .green = 0xff; .blue = 0xff; }
ws2812_t Color::red = { .red = 0xff; }
ws2812_t Color::orange = { .red = 0xff; .green = 0x7f; }
ws2812_t Color::yellow = { .red = 0xff; .green = 0xff; }
ws2812_t Color::chartreuse = { .red = 0x7f; .green = 0xff; }
ws2812_t Color::green = { .green = 0xff; }
ws2812_t Color::spring = { .green = 0xff; .blue = 0x7f; }
ws2812_t Color::cyan = { .green = 0xff; .blue = 0xff; }
ws2812_t Color::azure = { .green = 0x7f; .blue = 0xff; }
ws2812_t Color::blue = { .blue = 0xff; }
ws2812_t Color::violet = { .red = 0x7f; .blue = 0xff; }
ws2812_t Color::magenta = { .red = 0xff; .blue = 0xff; }
ws2812_t Color::rose = { .red = 0xff; .blue = 0x7f; }

inline float complementary(float c) {
	return (c + 180) % 360;
}
inline float triad(float c) {
	return (c + 120) % 360;
}
inline float analagous_a(float c) {
	return (c + 30) % 360;
}
inline float analagous_b(float c) {
	return (c + 330) % 360;
}
inline float split_a(float c) {
	return (c + 150) % 360;
}
inline float split_b(float c) {
	return (c + 210) % 360;
}

/*
 * the tetradic colors are
 * a = x
 * b = tetradic(x)
 * c = complementary(x)
 * d = tetradic(c)
 */
inline float tetradic(float c) {
	return (c + 120) % 360;
}
inline float square(float c) {
	return (c + 90) % 360;
}

void Color::h2rgb(struct ws2812_t *rgb, float h) {
	h /= 60;			// sector 0 to 5
	int i = floor(h);
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