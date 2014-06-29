#include "Point.h"

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

void Point::rotateXY(double angle){
	//Use rotational matrix to rotate point c1,c2 around the origin, resulting in new point n1,n2
	int16_t x1 = this->x * cos_f(angle) - this->y * sin_f(angle);
	int16_t y1 = this->x * sin_f(angle) + this->y * cos_f(angle);

	this->x = x1;
	this->y = y1;
}

void Point::rotateXYZ(Point v, double angle){
	
	
}