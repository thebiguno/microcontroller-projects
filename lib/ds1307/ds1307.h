#include "../time.h"

#ifndef ds1307_h
#define ds1307_h

/*
 * Sends GRB values to a chain of WS2811 leds.
 * v a pointer to the first GRB tuple
 * s the number of GRB tuples
 */
time_t get();
void set(time_t);

#endif
