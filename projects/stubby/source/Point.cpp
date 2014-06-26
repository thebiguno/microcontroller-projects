#include "Point.h"

using namespace digitalcave;

Point::Point(){
}
Point::Point(int16_t x, int16_t y, int16_t z){
	this->x = x;
	this->y = y;
	this->z = z;
}

void Point::add(Point offset){
	this->x += offset.x;
	this->y += offset.y;
	this->z += offset.z;
}
