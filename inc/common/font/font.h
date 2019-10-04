#ifndef FONT_H
#define FONT_H

#define FONT_FIXED_WIDTH		0
#define FONT_VARIABLE_WIDTH		1

typedef struct font {
	uint8_t* font_data;
	uint8_t* font_widths;
	uint8_t* codepage;
	uint8_t width;
	uint8_t height;
	uint8_t variable_width;
} font_t;

#endif
