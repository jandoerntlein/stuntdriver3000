#ifndef PARTICLES_H
#define PARTICLES_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <iostream>
#include "player.h"
#include <string>
#include "timer.h"
#include "light.h"
using namespace std;
using namespace ccgl;

class Particles {
protected:
    vec3f   m_pos;          // particle starting point
    vec3f   m_dir;          // movement direction
    vec3f   m_col;          // particle color

    int     m_nCount;       // number of particles
    int     m_alive;        // lifetime (ms)

    float   m_spread;       // spreading in direction m_dir
    float   m_scale;        // size
    float   m_speed;        // movement speed

    texture* m_tex;
    shader*  m_shader;
    mesh*    m_quad;         // quad 
    drawelement*            m_particles;

    matrix4x4f              trafo;

    Timer*  m_timer;
    double* m_startingTime; // save time when particles were created

    void init( string s_texture );
    void reset();

    vec3f*   currentPosition; // save each particles current position

    // explosion stuff
    bool is_explosion_mode;
    bool is_explosion_end;
    vec3f*   explosionDir;

public:
    Particles( vec3f& pos, vec3f& dir, vec3f& col,
               int nCount, float scale, float speed,
               float spread, int alive, string s_texture );
    ~Particles();
    void draw();
    void updatePos( vec3f& pos );
    void updateDir( vec3f& dir );
    void setExplosionMode();
};

class CarStripes : public Particles {
protected:
    Player* m_player;
    
    vector<vec3f> m_carpositions;
public:
    CarStripes( Player *p, vec3f& pos, vec3f& dir, vec3f& col,
               int nCount, float scale, float speed,
               float spread, int alive, string s_texture );
    ~CarStripes();
    void render();
};

class Fire : public CarStripes {
public:
    Fire( Player *p, vec3f& pos, vec3f& dir, vec3f& col,
               int nCount, float scale, float speed,
               float spread, int alive, string s_texture );
    ~Fire();
    void render();
};

#endif // PARTICLES_H
