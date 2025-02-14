#ifndef WOOD_H
#define WOOD_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <libccgl-0.0.1/objloader-addon.h>
#include <chipmunk/chipmunk.h>
#include "text2d.h"
#include "globalConstants.h"
#include "splinemesh.h"
#include "player.h"
#include "light.h"
#include <cmath>
#include <iostream>
using namespace std;
using namespace ccgl;

class Wood {
private:

    // graphic stuff
    vector<drawelement*> wood;
    float           m_rotationSpeed;

    int             m_type;

    Player*         m_player;
public:
    matrix4x4f trafo;
    float           getDistanceToPlayer();
    Wood( Player*, int type, vector<drawelement*>, float x, float y );
    ~Wood();
    void draw();
    void draw(texture_ref, matrix4x4f*);
};

class WoodContainer {
private:

    vector<Wood*>   m_Woods;
    SplineMesh*     m_spline;
    int             m_nWoods;

    Player*         m_player;
    float           m_playermindist;

    vector<drawelement*> mesh1;
    vector<drawelement*> mesh2;
    vector<drawelement*> mesh3;

public:
    WoodContainer( Player*, SplineMesh*, int );
    ~WoodContainer();
    void setBack(SplineMesh* s, int Barrels);
    void draw();
    void draw(texture_ref, matrix4x4f*);
};

#endif // WOOD_H
