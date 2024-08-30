#include "ContactNode.h"

/*	Zoe Johnston
 */

ContactNode::ContactNode() {
	position = NULL;
	vecSet(velocity, 0, 0, 0);
	vecSet(m_new_position, 0, 0, 0);
	vecSet(m_new_velocity, 0, 0, 0);
	vecSet(m_old_position, 0, 0, 0);
	vecSet(m_initial_position, 0, 0, 0);
	vecSet(force, 0, 0, 0);
	vecSet(compression, 0, 0, 0);
	normal = NULL;

	memset(neighbourhood, 0, sizeof(neighbourhood));

	u = 0;
	v = 0;
	du = 0;
	dv = 0;
	fu = 0;
	fv = 0;
	m_new_u = 0;
	m_new_v = 0;
	m_new_du = 0;
	m_new_dv = 0;
	m_old_u = 0;
	m_old_v = 0;
	m_initial_u = 0;
	m_initial_v = 0;

	fixed = 0;
	mass_u = 0;
	mass_v = 0;

	anchor_u = 0;
	anchor_v = 0;
}

void ContactNode::init(double* p, double* n) {
	position = p;
	normal = n;
	vecSet(normal, 0, 1, 0);
}

void ContactNode::set(double x, double y, double z, double un, double vn) {
	vecSet(position, x, y, z);
	vecCopy(m_initial_position, position);
	vecCopy(m_old_position, position);
	u = un;
	v = vn;
	m_initial_u = u;
	m_initial_v = v;
	m_old_u = u;
	m_old_v = v;
	anchor_u = un;
	anchor_v = vn;
}

void ContactNode::reset() {
	vecCopy(position, m_initial_position);
	vecCopy(m_old_position, m_initial_position);
	vecSet(velocity, 0, 0, 0);
	u = m_initial_u;
	v = m_initial_v;
	m_old_u = u;
	m_old_v = v;
	anchor_u = u;
	anchor_v = v;
}

void ContactNode::simulate(double* new_position, double* new_velocity, double new_u, double new_v, double new_du, double new_dv) {
	vecCopy(m_new_position, new_position);
	vecCopy(m_new_velocity, new_velocity);
	m_new_u = new_u;
	m_new_v = new_v;
	m_new_du = new_du;
	m_new_dv = new_dv;
}

void ContactNode::update() {
	m_old_u = u;
	m_old_v = v;
	vecCopy(m_old_position, position);

	vecCopy(position, m_new_position);
	vecCopy(velocity, m_new_velocity);
	u = m_new_u;
	v = m_new_v;
	du = m_new_du;
	dv = m_new_dv;
}
