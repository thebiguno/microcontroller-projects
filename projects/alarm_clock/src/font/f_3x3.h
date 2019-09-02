#ifndef FONT_3X3_H
#define FONT_3X3_H

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <font/font.h>

#include "cp_ascii_caps.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern font_t font_3x3;

#endif
