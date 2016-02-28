#include "dcmath.h"

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
// with changes from http://stackoverflow.com/questions/15818906/does-this-pointer-casting-break-strict-aliasing-rule
// to fix a warning about strict aliasing rule being broken
float invSqrt(float x){
	float halfx = 0.5f * x;
	float y = x;
	uint32_t i = (union { float f; uint32_t l; }) {y} .l;
	i = 0x5f3759df - (i>>1);
	y = (union { uint32_t l; float f; }) {i} .f;
	y = y * (1.5f - (halfx * y * y));
	return y;
}