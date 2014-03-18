#ifndef BITMAPS_H
#define BITMAPS_H

#include <avr/io.h>
#include <avr/pgmspace.h> 

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern uint8_t pumpkin[];
extern uint8_t skeleton[];

#define PUMPKIN_WIDTH	16
#define PUMPKIN_HEIGHT	16


#endif