#ifndef POINT_H
#define POINT_H

#include <avr/io.h>

#include "util/trig.h"


class Point {
	public:
		Point();
		Point(int16_t x, int16_t y, int16_t z);
		
		/*
		 * Adds the specified offset point to this point.
		 */
		void add(Point offset);

		/*
		 * Subtracts the specified offset point from this point.
		 */
		void subtract(Point offset);
		
		/*
		 * Rotate this point by specified angle on the x,y plane 
		 * around 0,0
		 */
		void rotateXY(double angle);

		/*
		 * Rotate this point by specified angle on the x,z plane 
		 * around 0,0
		 */
		void rotateXZ(double angle);
		
		/*
		 * Rotate this point by specified angle on the y,z plane 
		 * around 0,0
		 */
		void rotateYZ(double angle);
		
		/*
		 * Rotate this point by the specified angle around 
		 * the axis intercepting points 0,0,0 and v.
		 */
		void rotateXYZ(Point v, double angle);

		int16_t x;
		int16_t y;
		int16_t z;
};

#endif
