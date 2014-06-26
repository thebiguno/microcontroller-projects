#ifndef POINT_H
#define POINT_H

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#else
#include "../simulation/debug.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#endif

namespace digitalcave {
	class Point {
		public:
			Point();
			Point(int16_t x, int16_t y, int16_t z);
			
			/*
			 * Adds the specified offset point to this point.
			 */
			void add(Point offset);

			int16_t x;
			int16_t y;
			int16_t z;
	} ;
}

#endif
