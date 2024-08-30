#include "vector.h"

void vecSet(vec3 v, double a, double b, double c) {
    v[0] = a;
    v[1] = b;
    v[2] = c;
    return;
}

void vecAdd(vec3 c, const vec3 a, const vec3 b) {
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
}

void vecSubtract(vec3 c, const vec3 a, const vec3 b) {
	c[0] = a[0] - b[0];
	c[1] = a[1] - b[1];
	c[2] = a[2] - b[2];
}

void vecCopy(vec3 c, const vec3 a) {
    c[0] = a[0];
    c[1] = a[1];
    c[2] = a[2];
}

double vecDotProd(const vec3 a, const vec3 b) {
    return(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void vecCrossProd(vec3 c, const vec3 a, const vec3 b) {
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = -a[0] * b[2] + a[2] * b[0];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void vecNumMul(vec3 c, const vec3 a, double n) {
	c[0] = a[0] * n;
	c[1] = a[1] * n;
	c[2] = a[2] * n;
}

void vecScale(vec3 c, double n) {
	c[0] *= n;
	c[1] *= n;
	c[2] *= n;
}

double vecLength(const vec3 v) {
	return sqrt((v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

double vecLengthSquared(const vec3 v) {
	return (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vecNormalize(vec3 v) {
	double	norm;

	if ((norm = vecLength(v)) < 1e-6)
		return;

	norm = 1.0 / norm;
	v[0] *= norm;
	v[1] *= norm;
	v[2] *= norm;
	return;
}
