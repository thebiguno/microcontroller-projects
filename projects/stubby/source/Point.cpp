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

void rotate2D(int16_t* c1, int16_t* c2, double angle){
	//Use rotational matrix to rotate point c1,c2 around the origin, resulting in new point n1,n2
	int16_t n1 = *c1 * cos_f(angle) - *c2 * sin_f(angle);
	int16_t n2 = *c1 * sin_f(angle) + *c2 * cos_f(angle);
	
	*c1 = n1;
	*c2 = n2;
}

void Point::rotateXY(double angle){
	rotate2D(&this->x, &this->y, angle);
}

void Point::rotateXZ(double angle){
	rotate2D(&this->z, &this->x, angle);
}

void Point::rotateYZ(double angle){
	rotate2D(&this->y, &this->z, angle);
}

void Point::rotateXYZ(Point v, double angle){
	
	
}
