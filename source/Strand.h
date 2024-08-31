#ifndef MY_STRAND_H
#define MY_STRAND_H

#include "ContactNode.h"

/*	Zoe Johnston
 */

class Strand {
public:
	Strand();
	void init(int num, double len, int warpweft);

	ContactNode** nodes;
	int* inds;

	double density;
	double radius;
	double length;
	int num_nodes;
	int warp;

protected:
	int m_direction;
};

#endif
