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
		Color(uint16_t);
		Color(uint16_t h, uint8_t s, uint8_t v);
		Color(const Color &);
		~Color();
		
		uint16_t getHue();
		void setHue(uint16_t h);
		void addHue(int16_t h);

		uint8_t getSaturation();
		void setSaturation(uint8_t s);
		void addSaturation(int8_t s);

		uint8_t getValue();
		void setValue(uint8_t v);
		void addValue(int8_t v);

		pixel_t rgb();
		
		/* Alters the hue 180 degrees from the current hue. */
		void complementary();
		/* Alters the hue 120 degrees from the current hue.  Call twice to get three hues total or use in conjunction with complementary to get four tetradic hues. */
		void triad();
		/* Alters the hue 30 degrees from the current hue.  Use in conjunction with analagous_b to get three hues total. */
		void analagous_a();
		/* Alters the hue 330 degrees from the current hue.  Use in conjunction with analagous_a to get three hues total. */
		void analagous_b();
		/* Alters the hue 150 degrees from the current hue.  Use in conjunction with split_b to get three hues total. */
		void split_a();
		/* Alters the hue 210 degrees from the current hue.  Use in conjunction with split_a to get three hues total. */
		void split_b();
		/* Alters the hue 90 degrees from the current hue.  Call thrice to get four hues total. */
		void square();

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