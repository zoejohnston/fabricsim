#include "Strand.h"

/*	Zoe Johnston
 */

Strand::Strand() {
	nodes = NULL;
	inds = NULL;
	num_nodes = 0;
	m_direction = 0;
	density = 1;
	warp = 1;
	length = 1;
	radius = 0.2;
}

void Strand::init(int num, double len, int warpweft) {
	if (nodes)
		return;

	num_nodes = num;
	nodes = new ContactNode * [num_nodes];
	inds = new int[num_nodes];
	length = len;

	warp = warpweft;
}
