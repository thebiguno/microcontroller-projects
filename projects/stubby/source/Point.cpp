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

	//Translate back
	this->x = x1 + origin.x;
	this->y = y1 + origin.y;

void Point::rotateXYZ(Point v, double angle){
	//See http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/ArbitraryAxisRotation.html point 6.1
	// for how to do this.  This code has been written from these steps (or more accurately, blindly
	// stolen from the matrix solution without any understanding of how it works).
	
	//These are used multiple times in these calculations... do them once each to save time.
	double temp0 = v.x * v.x + v.y * v.y + v.z * v.z;
	double temp1 = 0 - v.x * this->x - v.y * this->y - v.z * this->z;
	double temp2 = 1 - cos_f(angle);
	double temp3 = temp0 * cos_f(angle);
	
	int16_t x1 = ((0 - v.x * temp1) * temp2 + x * temp3 + sqrt(temp0) * (0 - v.z * this->y + v.y * this->z) * sin_f(angle)) / temp0;
	int16_t y1 = ((0 - v.y * temp1) * temp2 + y * temp3 + sqrt(temp0) * (0 - v.z * this->x + v.x * this->z) * sin_f(angle)) / temp0;
	int16_t z1 = ((0 - v.z * temp1) * temp2 + z * temp3 + sqrt(temp0) * (0 - v.y * this->x + v.x * this->y) * sin_f(angle)) / temp0;
	
	this->x = x1;
	this->y = y1;
	this->z = z1;
}