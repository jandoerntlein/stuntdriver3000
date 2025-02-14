#ifndef TEXT2D_H
#define TEXT2D_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <lib3dmath/matrix.h>
#include <libcgl/cgl.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <string>
#include <vector>
#include "globalConstants.h"
using namespace std;
using namespace ccgl;

class Text2D {
private:
	texture* 		m_fonttexture;
	mesh*			m_quad;
	shader_ptr    	m_shader;
	drawelement*	m_quadelement;

	matrix4x4f		trafo;

	unsigned int	m_width;
	unsigned int 	m_height;

	camera_ref		m_ortho;
	void createcamera();

	void init( int x, int y, string text, int textsize );

public:
	Text2D();
	~Text2D();
	void draw( int x, int y, string text, int textsize );
};

#endif // TEXT2D_H