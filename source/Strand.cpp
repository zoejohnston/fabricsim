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

void Strand::display() {
	if (!nodes)
		return;

	for (int i = 0; i + 1 < num_nodes; i++) {
		//glNormal3dv(nodes[i]->normal);
		//glVertex3dv(nodes[i]->position);
		//glNormal3dv(nodes[i + 1]->normal);
		//glVertex3dv(nodes[i + 1]->position);
	}
}

void Strand::display(int index) {
	if (!nodes)
		return;

	vec3 position, normal, offset;

	for (int i = 0; i + 1 < num_nodes; i++) {
		vecSubtract(normal, nodes[i + 1]->position, nodes[i]->position);

		if (((i + index) % 2 && warp) || (!((i + index) % 2) && !warp)) {
			vecNumMul(offset, nodes[i]->normal, -0.1);
			vecScale(normal, 0.1);
		} else {
			vecNumMul(offset, nodes[i]->normal, 0.1);
			vecScale(normal, -0.1);
		}

		vecAdd(normal, nodes[i]->normal, normal);
		vecNormalize(normal);
		//glNormal3dv(normal);
		vecAdd(position, nodes[i]->position, offset);
		//glVertex3dv(position);

		vecAdd(normal, nodes[i + 1]->normal, normal);
		vecNormalize(normal);
		//glNormal3dv(normal);
		vecSubtract(position, nodes[i + 1]->position, offset);
		//glVertex3dv(position);
	}
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
