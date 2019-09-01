#ifndef FONT_H
#define FONT_H

typedef struct font {
	uint8_t* font_data;
	uint8_t* font_widths;
	uint8_t* codepage;
	uint8_t width;
	uint8_t height;
} font_t;

#endif
