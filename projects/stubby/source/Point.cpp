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

void Point::rotateXY(double angle){
	this->rotateXY(Point(0,0,0), angle);
}

void Point::rotateXY(Point origin, double angle){
	//Translate x, y by o.x, o.y
	this->x -= origin.x;
	this->y -= origin.y;
	
	//Use rotational matrix to rotate point c1,c2 around the origin, resulting in new point n1,n2
	int16_t x1 = this->x * cos_f(angle) - this->y * sin_f(angle);
	int16_t y1 = this->x * sin_f(angle) + this->y * cos_f(angle);

	//Translate back
	this->x = x1 + origin.x;
	this->y = y1 + origin.y;
}