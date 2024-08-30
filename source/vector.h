#ifndef	MY_VECTOR_H
#define	MY_VECTOR_H

#include <cmath>
#include <iostream>

/*  Code modified from Victor Ng and Petros Faloutsos's D.A.N.C.E. (Dynamic Animation aNd Control Environment)
 *  Zoe Johnston, 2024
 */

typedef	double vec3[3];

void vecSet(vec3 v, double a, double b, double c);
void vecAdd(vec3 c, const vec3 a, const vec3 b);
void vecSubtract(vec3 c, const vec3 a, const vec3 b);
void vecCopy(vec3 c, const vec3 a);
double vecDotProd(const vec3 a, const vec3 b);
void vecCrossProd(vec3 c, const vec3 a, const vec3 b);
void vecNumMul(vec3 c, const vec3 a, double n);
void vecScale(vec3 c, double	n);
double vecLength(const vec3 v);
double vecLengthSquared(const vec3 v);
void vecNormalize(vec3 v);

#endif
