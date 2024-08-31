#ifndef MY_FABRIC_H
#define MY_FABRIC_H

#include "ContactNode.h"
#include "Strand.h"
#include <cmath>
#include <iostream>

/*	Zoe Johnston
 */

class Fabric {
public:
	Fabric(int width, int height, double* vertices, unsigned int* indices);
	~Fabric();
	void reset(double time);
	void step();
	void fix(int i, int j);

protected:
	int m_num_strands;
	int m_num_warps;
	int m_num_nodes;
	Strand* m_strands;
	ContactNode* m_nodes;

	double friction_limit;
	double sinch;
};

#endif
