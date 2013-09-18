#ifndef FONT_SMALL_H
#define FONT_SMALL_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t font_small[];

#define FONT_SMALL_WIDTH	5
#define FONT_SMALL_HEIGHT	7

#endif