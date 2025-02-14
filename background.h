#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "globalConstants.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <iostream>
#include <string>
#include "light.h"
#include "player.h"
#include "globalConstants.h"


using namespace std;
using namespace ccgl;

class Background {
private:
	shader*      backgroundShader;
	
	drawelement* background1;
	drawelement* background2;
	
	texture*     m_tex1;
	texture*     m_tex2;

	matrix4x4f   trafo1;
	matrix4x4f   trafo2;
	float        m_scale1;
	float        m_scale2;

	Cloud**		m_clouds;    
	int clouds;


	Player*     player;
	vec3f		pos1;
	vec3f		pos2;
public:
	Background(Player* player);
	~Background();
	void draw();
};

#endif // BACKGROUND_H
