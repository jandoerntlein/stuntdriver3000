#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <iostream>
#include <string>
#include "timer.h"
#include "light.h"
using namespace std;

using namespace ccgl;

class Hud {
protected:
    float   m_scale;        // size
    
    shader*  m_shader;
    mesh*    m_quad_fg;         // quad 
    mesh*    m_quad_bg;         // quad 
    drawelement*             m_element;

    matrix4x4f              trafo;

    vec2f   m_pos;

public:
    Hud();
    ~Hud();
    void draw( float amount );
};

#endif // HUD_H
