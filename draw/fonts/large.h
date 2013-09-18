#ifndef FONT_LARGE_H
#define FONT_LARGE_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t codepage_large[];

extern uint8_t font_large[];

#define FONT_LARGE_WIDTH	7
#define FONT_LARGE_HEIGHT	11

#endif