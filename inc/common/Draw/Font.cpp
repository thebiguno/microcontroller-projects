#include "Font.h"

using namespace digitalcave;

Font::Font(Stream* codepage, Icon* glyphs) :
	codepage(codepage),
	glyphs(glyphs) {}

Font::~Font() {}

void Font::text(const char* text, Draw* draw, int16_t x, int16_t y, uint8_t orientation) {
	uint8_t i = 0;

	while (text[i]) {
		this->character(text[i], draw, x, y, orientation);
		i++;
		if (orientation == DRAW_ORIENTATION_NORMAL) x += (glyphs->getWidth() + 1);
		else if (orientation == DRAW_ORIENTATION_DOWN) y += (glyphs->getWidth() + 1);
	}
}

void Font::character(char c, Draw* draw, int16_t x, int16_t y, uint8_t orientation) {
	//Find the entry in the code page
	uint8_t glyph_index = 0xff;
	codepage->reset();
	codepage->skip((uint8_t) c);
	codepage->read(&glyph_index);

	if (glyph_index != 0xFF) {
		glyphs->setFrame(glyph_index);
		glyphs->draw(draw, x, y, orientation);
	}
}
