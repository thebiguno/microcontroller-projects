#ifndef CLOCK_H
#define CLOCK_H

#include "lib/draw/draw.h"
#include "lib/time/time.h"

#define GRN_3	0x0C
#define GRN_2	0x08
#define GRN_1	0x04
#define RED_3	0x03
#define RED_2	0x02
#define RED_1	0x01

void clock_draw(struct time_t *t);

#endif