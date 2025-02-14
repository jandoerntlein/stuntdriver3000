#ifndef POPUP_H
#define POPUP_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <iostream>
#include <string>
#include "timer.h"
#include "light.h"
#include "globalConstants.h"
using namespace std;

using namespace ccgl;

class Popup {
protected:
    float   m_scale;        // size
    
    texture* m_tex;
    shader*  m_shader;
    mesh*    m_quad;         // quad 
    drawelement*             m_element;

    matrix4x4f              trafo;

    Timer*  m_timer;

public:
    vec2f   m_pos;

    Popup( float scale, string s_texture );
    ~Popup();
    void draw();
};

class Cloud : public Popup {
private:
    int update_ticker;
    void randomDir();
public:
    vec2f m_dir;

    Cloud( float scale, string s_texture ) : Popup( scale, s_texture ) {
        m_dir = { 0, 0 };
        update_ticker = 0;
        randomDir();
    };
    void render();
};

#endif // POPUP_H
