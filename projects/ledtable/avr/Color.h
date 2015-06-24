#ifndef Menu_H
#define Menu_H

#include <stdint.h>
#include "matrix.h"

namespace digitalcave {
	class Color {
	private:
		uint16_t h;
		uint8_t s;
		uint8_t v;
	public:
		Color(uint16_t h);
		Color(uint16_t h, uint8_t s, uint8_t v);
		Color(Color c);
		~Color();
		
		uint16_t getHue();
		void setHue(uint16_t h);

		uint8_t getSaturation();
		void setSaturation(uint8_t s);

		uint8_t getValue();
		void setValue(uint8_t h);

		pixel_t rgb();
		
		/* Returns the hue 180 degrees from the current hue. */
		Color complementary();
		/* Returns the hue 120 degrees from the current hue.  Call twice to get three hues total or use in conjunction with complementary to get four tetradic hues. */
		Co triad(uint16_t c);
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
		static const uint16_t ORANGE = 30;
		static const uint16_t YELLOW = 60;
		static const uint16_t CHARTREUSE = 90;
		static const uint16_t GREEN = 120;
		static const uint16_t SPRING = 150;
		static const uint16_t CYAN = 180;
		static const uint16_t AZURE = 210;
		static const uint16_t BLUE = 240;
		static const uint16_t VIOLET = 270;
		static const uint16_t MAGENTA = 300;
		static const uint16_t ROSE = 330;
	};
}

#endif