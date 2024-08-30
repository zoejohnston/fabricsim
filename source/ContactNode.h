#ifndef MY_CONTACT_NODE_H
#define MY_CONTACT_NODE_H

#include "vector.h"

/*	Zoe Johnston
 */

class ContactNode {
public:
	ContactNode();
	void init(double* p, double* n);
	void set(double x, double y, double z, double un, double vn);
	void reset();
	void simulate(double* new_position, double* new_velocity, double new_u, double new_v, double new_du, double new_dv);
	void update();

	double* position;
	double u;
	double v;

	vec3 velocity;
	double du;
	double dv;

	int fixed;

	vec3 force;
	vec3 compression;
	double* normal;
	double mass_u;
	double mass_v;
	double fu;
	double fv;

	vec3 m_old_position;
	double m_old_u;
	double m_old_v;

	double anchor_u;
	double anchor_v;

	int neighbourhood[9];

	double m_new_u;
	double m_new_v;

protected:
	vec3 m_initial_position;
	double m_initial_u;
	double m_initial_v;

	vec3 m_new_position;

	vec3 m_new_velocity;
	double m_new_du;
	double m_new_dv;
};

#endif
