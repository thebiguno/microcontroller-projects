#ifndef FONT_MEDIUM_H
#define FONT_MEDIUM_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t font_medium[];

#define FONT_MEDIUM_WIDTH	7
#define FONT_MEDIUM_HEIGHT	9

#endif