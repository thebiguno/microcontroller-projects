#ifndef FONT_XXSMALL_H
#define FONT_XXSMALL_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t font_xsmall[];

#define FONT_XXSMALL_WIDTH		3
#define FONT_XXSMALL_HEIGHT		3

#endif