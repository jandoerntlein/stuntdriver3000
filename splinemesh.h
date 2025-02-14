#ifndef SPLINEMESH_H
#define SPLINEMESH_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <chipmunk/chipmunk.h>
#include <libcgl/cgl.h>
#include "spline.h"
#include <libcgl/shader.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include "light.h"
#include "globalConstants.h"

using namespace std;
using namespace ccgl;

class SplineMesh : public Spline {
private:
	shader* splineShader;
	mesh* m_spline;
	drawelement* drawableSpline;
	texture* m_tex;

	cpBody*		m_body;		// static body
	cpShape*    m_shape;	// collision shapes

public:
	SplineMesh( cpSpace* );
	void addCollider( cpSpace* );
	~SplineMesh();
	Spline getUsedSpline();
	void setup_dir_light(ccgl::shader_ptr shader);
	void draw();
	mesh_ref makeLineMesh(Spline s);
	void makeTrianglesMesh(Spline s, Spline second);
	void makeDrawable();
	matrix4x4f trafo;
	vector<vec3f> spline_strip;
};

#endif // SPLINEMESH_H
