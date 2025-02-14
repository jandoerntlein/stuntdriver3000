#ifndef SHADOW_H
#define SHADOW_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <iostream>
#include <string>
#include "timer.h"
#include "view.h"
#include "player.h"
using namespace std;
using namespace ccgl;

// #define DEBUG_MODE

class Shadow {
protected:
    Player*         m_player;
    camera_ref      m_cam;
    texture_ref     m_depth;
    framebuffer_ref m_fbo;

    camera_ref      m_main_cam;
    matrix4x4f*     m_matT;

    vec3f           m_pos;
    vec3f           m_dir;
    vec3f           m_up;

    void updatePos();
public:
    Shadow( Player*, texture_ref );
    ~Shadow();
    texture_ref depthtexture();
    matrix4x4f* T();
    void begin(framebuffer_ref);
    void end(framebuffer_ref);
};

#endif // SHADOW_H
