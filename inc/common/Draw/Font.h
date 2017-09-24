#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <Draw.h>
#include <Icon.h>
#include <Stream.h>

namespace digitalcave {
	class Font {
	private:
		Stream* codepage;
		Icon* glyphs;

	public:
		Font(Stream* codepage, Icon* glyphs);
		~Font();

		void text(const char* text, Draw *draw, int16_t x, int16_t y, uint8_t orientation);
		void character(char c, Draw *draw, int16_t x, int16_t y, uint8_t orientation);
	};
}

#endif
