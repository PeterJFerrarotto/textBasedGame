#ifndef VECTOR_3_H
#define VECTOR_3_H
#define _USE_MATH_DEFINES
#include <math.h>
struct Vector3{
	float x;
	float y;
	float z;

	inline void normalize(){
		float length = sqrt(x*x + y*y);
		x /= length;
		y /= length;
	}
};
#endif