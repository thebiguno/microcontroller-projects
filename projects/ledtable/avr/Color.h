#ifndef Menu_H
#define Menu_H

#include <stdint.h>
#include "matrix.h"

namespace digitalcave {
	class Color {
	public:
		void h2rgb(float hue, struct ws2812_t *rgb);
		
		/* Returns the hue 180 degrees from the current hue. */
		uint16_t complementary(uint16_t h);
		/* Returns the hue 120 degrees from the current hue.  Call twice to get three hues total or use in conjunction with complementary to get four tetradic hues. */
		uint16_t triad(uint16_t c);
		/* Returns the hue 30 degrees from the current hue.  Use in conjunction with analagous_b to get three hues total. */
		uint16_t analagous_a(uint16_t c);
		/* Returns the hue 330 degrees from the current hue.  Use in conjunction with analagous_a to get three hues total. */
		uint16_t analagous_b(uint16_t c);
		/* Returns the hue 150 degrees from the current hue.  Use in conjunction with split_b to get three hues total. */
		uint16_t split_a(uint16_t c);
		/* Returns the hue 210 degrees from the current hue.  Use in conjunction with split_a to get three hues total. */
		uint16_t split_b(uint16_t c);
		/* Returns the hue 90 degrees from the current hue.  Call thrice to get four hues total. */
		uint16_t square(uint16_t c);

		/*
		 * the tetradic colors are
		 * a = x
		 * b = triad(x)
		 * c = complementary(x)
		 * d = triad(c)
		 */
		
		static const uint16_t RED = 0;
		static const uint16_t orange = 30;
		static const uint16_t yellow = 60;
		static const uint16_t chartreuse = 90;
		static const uint16_t green = 120;
		static const uint16_t spring = 150;
		static const uint16_t cyan = 180;
		static const uint16_t azure = 210;
		static const uint16_t blue = 240;
		static const uint16_t violet = 270;
		static const uint16_t magenta = 300;
		static const uint16_t rose = 330;
	};
}

#endif