#ifndef FONT_XLARGE_H
#define FONT_XLARGE_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t codepage_xlarge[];

extern uint8_t font_xlarge[];

#define FONT_XLARGE_WIDTH	11
#define FONT_XLARGE_HEIGHT	17

#endif