#include "Fabric.h"
#define TIMESTEP 0.005

/*	Zoe Johnston
 */

void setNeighbours(Strand* strands, int num_warps) {
	for (int i = 0; i < num_warps; i++) {
		for (int j = 0; j < strands[i].num_nodes; j++) {
			for (int k = 0; k < 9; k++)
				strands[i].nodes[j]->neighbourhood[k] = strands[i].inds[j];

			if (i - 1 >= 0 && j < strands[i - 1].num_nodes) {
				strands[i].nodes[j]->neighbourhood[1] = strands[i - 1].inds[j];
				if (j + 1 < strands[i - 1].num_nodes)
					strands[i].nodes[j]->neighbourhood[5] = strands[i - 1].inds[j + 1];
				if (j - 1 >= 0)
					strands[i].nodes[j]->neighbourhood[6] = strands[i - 1].inds[j - 1];
			}
			if (i + 1 < num_warps && j < strands[i + 1].num_nodes) {
				strands[i].nodes[j]->neighbourhood[2] = strands[i + 1].inds[j];
				if (j + 1 < strands[i + 1].num_nodes)
					strands[i].nodes[j]->neighbourhood[7] = strands[i + 1].inds[j + 1];
				if (j - 1 >= 0)
					strands[i].nodes[j]->neighbourhood[8] = strands[i + 1].inds[j - 1];
			}
			if (j - 1 >= 0)
				strands[i].nodes[j]->neighbourhood[3] = strands[i].inds[j - 1];
			if (j + 1 < strands[i].num_nodes)
				strands[i].nodes[j]->neighbourhood[4] = strands[i].inds[j + 1];
		}
	}
}

Fabric::Fabric(int width, int height, double* vertices, unsigned int* indices) {
	friction_limit = 20;
	sinch = 0.0;

	if (height <= 0 || width <= 0) {
		m_num_strands = 0;
		m_num_warps = 0;
		m_num_nodes = 0;
		m_strands = NULL;
		m_nodes = NULL;
		return;
	}

	m_num_strands = width + height;
	m_num_nodes = width * height;
	m_num_warps = height;
	m_strands = new Strand[m_num_strands];
	m_nodes = new ContactNode[m_num_nodes];

	unsigned int count = 0;

	for (int i = 0; i < height; i++) {
		m_strands[i].init(width, width - 1, 1);
		for (int j = 0; j + 1 < width; j++) {
			m_strands[i].nodes[j] = &m_nodes[(width * i) + j];
			m_strands[i].inds[j] = (width * i) + j;
			indices[count] = (width * i) + j;
			indices[count + 1] = (width * i) + j + 1;
			count += 2;
		}
		m_strands[i].nodes[width - 1] = &m_nodes[(width * i) + width - 1];
		m_strands[i].inds[width - 1] = (width * i) + width - 1;
	}
	for (int j = 0; height + j < m_num_strands; j++) {
		m_strands[height + j].init(height, height - 1, 0);
		for (int i = 0; i + 1 < height; i++) {
			m_strands[height + j].nodes[i] = &m_nodes[(width * i) + j];
			m_strands[height + j].inds[i] = (width * i) + j;
			indices[count] = (width * i) + j;
			indices[count + 1] = (width * (i + 1)) + j;
			count += 2;
		}
		m_strands[height + j].nodes[height - 1] = &m_nodes[(width * (height - 1)) + j];
		m_strands[height + j].inds[height - 1] = (width * (height - 1)) + j;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; (width * i) + j < m_num_nodes; j++) {
			m_nodes[(width * i) + j].init(&vertices[6 * ((width * i) + j)], &vertices[6 * ((width * i) + j) + 3]);
			m_nodes[(width * i) + j].set(i, 0, j, (double)j / (width - 1), (double)i / (height - 1));
		}
	}

	setNeighbours(m_strands, m_num_strands);
}

Fabric::~Fabric() {
	delete[] m_strands;
	delete[] m_nodes;
}

void Fabric::fix(int i, int j) {
	if (i < 0 || j < 0 || i > m_num_strands || j > m_strands[i].num_nodes)
		return;

	m_strands[i].nodes[j]->fixed = 1;
}

void Fabric::reset(double time){
	for (int i = 0; i < m_num_nodes; i++) {
		m_nodes[i].reset();
	}
}

void gravity(ContactNode* p) {
	vec3 temporary;
	vecSet(temporary, 0, -10 * (p->mass_u + p->mass_v), 0);
	vecAdd(p->force, p->force, temporary);
}

void drag(double* force, ContactNode* p) {
	vec3 temporary;
	vecNumMul(temporary, p->velocity, -5);
	vecAdd(force, force, temporary);
}

void spring(ContactNode* p0, ContactNode* p1, double distance, int warp) {
	vec3 temporary, force;
	vecSubtract(temporary, p1->position, p0->position);
	vecScale(temporary, 1 / distance);
	double mult = distance * (vecLength(temporary) - 1);

	vecNormalize(temporary);

	vecNumMul(force, temporary, 10000 * mult);
	vecAdd(p0->force, p0->force, force);

	vecNumMul(force, temporary, 10000 * mult);
	vecSubtract(p1->force, p1->force, force);

	if (warp) {
		vecAdd(p0->compression, p0->compression, force);
		vecSubtract(p1->compression, p1->compression, force); 
		p0->fu -= mult;
		p1->fu += mult;
	} else {
		vecSubtract(p0->compression, p0->compression, force);
		vecAdd(p1->compression, p1->compression, force);
		p0->fv -= mult;
		p1->fv += mult;
	}

	vecSubtract(force, p1->velocity, p0->velocity);
	vecScale(temporary, 30 * vecDotProd(force, temporary));
	vecAdd(p0->force, p0->force, temporary);
	vecSubtract(p1->force, p1->force, temporary);
}

void stretch(ContactNode* nodes, Strand* strands, int num_strands) {
	for (int i = 0; i < num_strands; i++) {
		for (int j = 0; j + 1 < strands[i].num_nodes; j++) {
			double distance;
			int i0 = strands[i].inds[j];
			int i1 = strands[i].inds[j + 1];

			if (strands[i].warp) {
				distance = strands[i].length * (strands[i].nodes[j + 1]->u - strands[i].nodes[j]->u);
				nodes[i0].mass_u += strands[i].density * distance / 2;
				nodes[i1].mass_u += strands[i].density * distance / 2;
				spring(&nodes[i0], &nodes[i1], distance, 1);
			} else {
				distance = strands[i].length * (strands[i].nodes[j + 1]->v - strands[i].nodes[j]->v);
				nodes[i0].mass_v += strands[i].density * distance / 2;
				nodes[i1].mass_v += strands[i].density * distance / 2;
				spring(&nodes[i0], &nodes[i1], distance, 0);
			}
		}
	}
}

void normal(double* result, double* up, double* down, double* left, double* right) {
	vec3 x, y;
	vecSubtract(x, right, left);
	vecSubtract(y, down, up);
	vecCrossProd(result, x, y);
	vecNormalize(result);
}

void normals(ContactNode* nodes, Strand* strands, int num_warps) {
	if (num_warps < 2)
		return;

	ContactNode *center, *up, *down, *left, *right;

	for (int i = 0; i < num_warps; i++) {
		for (int j = 0; j < strands[i].num_nodes; j++) {
			center = strands[i].nodes[j];
			up = strands[i].nodes[j];
			down = strands[i].nodes[j];
			left = strands[i].nodes[j];
			right = strands[i].nodes[j];

			if (i - 1 >= 0 && j < strands[i - 1].num_nodes)
				up = strands[i - 1].nodes[j];
			if (i + 1 < num_warps && j < strands[i + 1].num_nodes)
				down = strands[i + 1].nodes[j];
			if (j - 1 >= 0)
				left = strands[i].nodes[j - 1];
			if (j + 1 < strands[i].num_nodes)
				right = strands[i].nodes[j + 1];

			normal(center->normal, up->position, down->position, left->position, right->position);
		}
	}
}

void compression(ContactNode* nodes, Strand* strands, int num_strands, int num_warps) {
	for (int i = 0; i < num_strands; i++) {
		for (int j = 1; j + 1 < strands[i].num_nodes; j++) {
			vec3 offset;
			vec3 p0, p1, p2;
			int i0 = strands[i].inds[j - 1];
			int i1 = strands[i].inds[j];
			int i2 = strands[i].inds[j + 1];

			if (((i + j) % 2 && strands[i].warp) || !((i + j) % 2 || strands[i].warp)) {
				vecNumMul(offset, strands[i].nodes[j - 1]->normal, 0.2);
				vecAdd(p0, nodes[i0].position, offset);
				vecNumMul(offset, strands[i].nodes[j]->normal, -0.2);
				vecAdd(p1, nodes[i1].position, offset);
				vecNumMul(offset, strands[i].nodes[j + 1]->normal, 0.2);
				vecAdd(p2, nodes[i2].position, offset);
			} else {
				vecNumMul(offset, strands[i].nodes[j - 1]->normal, -0.2);
				vecAdd(p0, nodes[i0].position, offset);
				vecNumMul(offset, strands[i].nodes[j]->normal, 0.2);
				vecAdd(p1, nodes[i1].position, offset);
				vecNumMul(offset, strands[i].nodes[j + 1]->normal, -0.2);
				vecAdd(p2, nodes[i2].position, offset);
			}

			vec3 a, b, c, f0, f1, f2;
			vecSubtract(a, p0, p1);
			vecSubtract(b, p1, p2);
			vecNormalize(a);
			vecNormalize(b);

			double angle = acos(vecDotProd(a, b));
			double distance;

			if (strands[i].warp) {
				distance = strands[i].length * (nodes[i2].u - nodes[i0].u);
			} else {
				distance = strands[i].length * (nodes[i2].v - nodes[i0].v);
			}

			double scale = 60 * angle / distance;

			if (isnan(scale))
				return;

			vecSubtract(a, p0, p1);
			vecSubtract(b, p2, p1);
			vecNormalize(a);
			vecNormalize(b);
			vecCrossProd(c, a, b);

			vecSubtract(f0, b, a);
			vecCrossProd(f1, f0, c);
			vecNormalize(f1);
			vecScale(f1, scale);

			vecCrossProd(f0, a, c);
			vecScale(f0, scale);
			vecCrossProd(f2, c, b);
			vecScale(f2, scale);

			if (strands[i].warp) {
				vecAdd(nodes[i1].compression, nodes[i1].compression, f1);
				vecAdd(nodes[i0].compression, nodes[i0].compression, f0);
				vecAdd(nodes[i2].compression, nodes[i2].compression, f2);
			} else {
				vecSubtract(nodes[i1].compression, nodes[i1].compression, f1);
				vecSubtract(nodes[i0].compression, nodes[i0].compression, f0);
				vecSubtract(nodes[i2].compression, nodes[i2].compression, f2);
			}
		}
	}
}

void friction(ContactNode* nodes, Strand* strands, int num_strands, int num_warps, int friction_limit) {
	normals(nodes, strands, num_warps);
	compression(nodes, strands, num_strands, num_warps);
	
	for (int i = 0; i < num_strands; i++) {
		for (int j = 1; j + 1 < strands[i].num_nodes; j++) {
			ContactNode* self = strands[i].nodes[j];
			vec3 compression;

			if ((i + j) % 2 == 0)
				vecNumMul(compression, self->compression, -1);
			else
				vecCopy(compression, self->compression);

			double Fn = 0.5 * vecDotProd(self->normal, compression);

			if (Fn < 0)
				Fn = 0;

			if (std::abs(self->u - self->anchor_u) >= friction_limit * Fn) {
				if (strands[i].warp) {
					self->fu -= 20 * self->du;
					if (self->u - self->anchor_u < 0) {
						self->fu += friction_limit * Fn;
						self->anchor_u = self->u + 0.0001;
					} else {
						self->fu -= friction_limit * Fn;
						self->anchor_u = self->u - 0.0001;
					}
				} else {
					self->fv -= 20 * self->dv;
					if (self->v - self->anchor_v < 0) {
						self->fv += friction_limit * Fn;
						self->anchor_v = self->v + 0.0001;
					} else {
						self->fv -= friction_limit * Fn;
						self->anchor_v = self->v - 0.0001;
					}
				}
			} else {
				if (strands[i].warp)
					self->fu -= 500 * (self->u - self->anchor_u) + 100 * self->du;
				else
					self->fv -= 500 * (self->v - self->anchor_v) + 100 * self->dv;
			}
		}
	}
}

void bending(ContactNode* nodes, Strand* strands, int num_strands) {
	for (int i = 0; i < num_strands; i++) {
		for (int j = 1; j + 1 < strands[i].num_nodes; j++) {
			int i0 = strands[i].inds[j - 1];
			int i1 = strands[i].inds[j];
			int i2 = strands[i].inds[j + 1];

			vec3 a, b, c, d, e;
			vecSubtract(a, nodes[i0].position, nodes[i1].position);
			vecSubtract(b, nodes[i1].position, nodes[i2].position);
			vecNormalize(a);
			vecNormalize(b);

			double angle = acos(vecDotProd(a, b));
			double distance;

			if (strands[i].warp) {
				distance = strands[i].length * (nodes[i2].u - nodes[i0].u);
			} else {
				distance = strands[i].length * (nodes[i2].v - nodes[i0].v);
			}

			double scale = 60 * angle / distance;

			if (isnan(scale))
				return;

			vecSubtract(a, nodes[i0].position, nodes[i1].position);
			vecSubtract(b, nodes[i2].position, nodes[i1].position);
			vecNormalize(a);
			vecNormalize(b);
			vecCrossProd(c, a, b);

			vecSubtract(d, b, a);
			vecCrossProd(e, d, c);
			vecNormalize(e);
			vecScale(e, scale);
			vecAdd(nodes[i1].force, nodes[i1].force, e);

			vecCrossProd(d, a, c);
			vecScale(d, scale);
			vecAdd(nodes[i0].force, nodes[i0].force, d);
			vecCrossProd(d, c, b);
			vecScale(d, scale);
			vecAdd(nodes[i2].force, nodes[i2].force, d);
		}
	}
}

void shearComponent(ContactNode* nodes, int i0, int i1, int i2) {
	vec3 a, b, c, d;
	vecSubtract(a, nodes[i0].position, nodes[i1].position);
	vecSubtract(b, nodes[i2].position, nodes[i1].position);
	vecNormalize(a);
	vecNormalize(b);

	double dot = vecDotProd(a, b);
	double angle = acos(dot);
	double scale = -120 * (angle - 1.57);

	vecCrossProd(c, a, b);
	vecCrossProd(d, a, c);
	vecScale(d, scale);
	vecAdd(nodes[i0].force, nodes[i0].force, d);
	vecCrossProd(d, c, b);
	vecScale(d, scale);
	vecAdd(nodes[i2].force, nodes[i2].force, d);
}

void shear(ContactNode* nodes, Strand* strands, int num_warps) {
	for (int i = 0; i + 1 < num_warps; i++) {
		for (int j = 0; j + 1 < strands[i].num_nodes; j++) {
			shearComponent(nodes, strands[i].inds[j], strands[i + 1].inds[j], strands[i + 1].inds[j + 1]);
			shearComponent(nodes, strands[i + 1].inds[j], strands[i + 1].inds[j + 1], strands[i].inds[j + 1]);
			shearComponent(nodes, strands[i + 1].inds[j + 1], strands[i].inds[j + 1], strands[i].inds[j]);
			shearComponent(nodes, strands[i].inds[j + 1], strands[i].inds[j], strands[i + 1].inds[j]);
		}
	}
}

void parallelPenalty(ContactNode* p0, ContactNode* p1, int length, int warp) {
	double distance;

	if (warp) {
		distance = length * (p1->u - p0->u);
		if (distance < 0.4) {
			p0->fu -= 500 * (0.4 - distance);
			p1->fu += 500 * (0.4 - distance);
		}
	} else {
		distance = length * (p1->v - p0->v);
		if (distance < 0.4) {
			p0->fv -= 500 * (0.4 - distance);
			p1->fv += 500 * (0.4 - distance);
		}
	}
}

void sliding(ContactNode* nodes, Strand* strands, int num_strands) {
	for (int i = 0; i < num_strands; i++) {
		for (int j = 0; j + 1 < strands[i].num_nodes; j++) {
			int i1 = strands[i].inds[j];
			int i2 = strands[i].inds[j + 1];

			parallelPenalty(&nodes[i1], &nodes[i2], strands[i].length, strands[i].warp);
		}
	}
}

void collisions(ContactNode* nodes, int num_nodes) {
	vec3 difference, temporary;
	double distance;
	int* n;

	for (int i = 0; i < num_nodes; i++) {
		n = nodes[i].neighbourhood;

		for (int j = 0; j < num_nodes; j++) {
			if (i <= j)
				continue;
			if (j == n[0] || j == n[1] || j == n[2] || j == n[3] || j == n[4] || j == n[5] || j == n[6] || j == n[7] || j == n[8])
				continue;

			vecSubtract(difference, nodes[i].position, nodes[j].position);
			distance = vecLengthSquared(difference);

			// 200 is good for 30x30, 500 for 40x40
			if (distance < 0.81) {
				vecNormalize(difference);
				vecNumMul(temporary, difference, 200 * (0.81 - distance));
				vecAdd(nodes[i].force, nodes[i].force, temporary);
				vecNumMul(temporary, difference, 200 * (0.81 - distance));
				vecSubtract(nodes[j].force, nodes[j].force, temporary);
			}
		}
	}
}

void vectorVerlet(double* position, double* velocity, ContactNode* p, ContactNode* nodes, int num_nodes) {
	vec3 temporary;
	vecNumMul(position, p->position, 2);
	vecSubtract(position, position, p->m_old_position);
	vecNumMul(temporary, p->force, (TIMESTEP * TIMESTEP) / (p->mass_u + p->mass_v));
	vecAdd(position, position, temporary);

	vecSubtract(velocity, position, p->m_old_position);
	vecScale(velocity, 1 / (TIMESTEP * 2));

	int* n = p->neighbourhood;
	double distance;

	for (int k = 0; k < num_nodes; k++) {
		if (k == n[0] || k == n[1] || k == n[2] || k == n[3] || k == n[4] || k == n[5] || k == n[6] || k == n[7] || k == n[8])
			continue;

		vecSubtract(temporary, nodes[k].position, position);
		distance = vecLengthSquared(temporary);

		if (distance < 0.49 && vecDotProd(temporary, velocity) < 0) {
			vecCopy(position, p->position);
			vecCopy(velocity, p->velocity);
		}
	}
}

void verlet(double* u, double* v, double* du, double* dv, ContactNode* p) {
	double new_u = (2 * p->u) - p->m_old_u + (TIMESTEP * TIMESTEP * p->fu / p->mass_v);
	double new_v = (2 * p->v) - p->m_old_v + (TIMESTEP * TIMESTEP * p->fv / p->mass_u);

	if (new_u > 1.0)
		*u = 1.0;
	else if (new_u < 0)
		*u = 0.0;
	else
		*u = new_u;

	if (new_v > 1.0)
		*v = 1.0;
	else if (new_v < 0)
		*v = 0.0;
	else
		*v = new_v;

	*du = (*u - p->m_old_u) / (2 * TIMESTEP);
	*dv = (*v - p->m_old_v) / (2 * TIMESTEP);
}

/* Steps the animation forward in time
 */
void Fabric::step() {
	if (!m_strands || !m_nodes)
		return;

	for (int i = 0; i < m_num_nodes; i++) {
		vecSet(m_nodes[i].force, 0, 0, 0);
		vecSet(m_nodes[i].compression, 0, 0, 0);
		m_nodes[i].mass_u = 0;
		m_nodes[i].mass_v = 0;
		m_nodes[i].fu = 0;
		m_nodes[i].fv = 0;
	}

	stretch(m_nodes, m_strands, m_num_strands);
	friction(m_nodes, m_strands, m_num_strands, m_num_warps, friction_limit);
	bending(m_nodes, m_strands, m_num_strands);
	shear(m_nodes, m_strands, m_num_warps);
	sliding(m_nodes, m_strands, m_num_strands);
	collisions(m_nodes, m_num_nodes);

	#pragma loop(hint_parallel(14))
	for (int i = 0; i < m_num_nodes; i++) {
		gravity(&m_nodes[i]);
		drag(m_nodes[i].force, &m_nodes[i]);
	}

	for (int i = 0; i < m_num_strands; i++) {
		if (m_strands[i].warp) {
			m_strands[i].nodes[0]->fu = 0;
			m_strands[i].nodes[m_strands[i].num_nodes - 1]->fu = 0;
		} else {
			m_strands[i].nodes[0]->fv = 0;
			m_strands[i].nodes[m_strands[i].num_nodes - 1]->fv = 0;
		}
	}

	/* uncomment to allow sinching */
	//m_strands[40].nodes[0]->fv = sinch;

	#pragma loop(hint_parallel(14))
	for (int i = 0; i < m_num_nodes; i++) {
		if (m_nodes[i].fixed) 
			vecSet(m_nodes[i].force, 0, 0, 0);

		vec3 position, velocity;
		double u, v, du, dv;

		vectorVerlet(position, velocity, &m_nodes[i], m_nodes, m_num_nodes);
		verlet(&u, &v, &du, &dv, &m_nodes[i]);

		m_nodes[i].simulate(position, velocity, u, v, du, dv);
	}

	for (int i = 0; i < m_num_nodes; i++)
		m_nodes[i].update();
}
