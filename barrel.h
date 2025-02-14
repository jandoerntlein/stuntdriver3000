#ifndef BARREL_H
#define BARREL_H

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
#include <iostream>
using namespace std;
using namespace ccgl;

class Barrel {
private:

    // graphic stuff
    vector<drawelement*> barrel;
    float           m_rotationSpeed;

    void            update();
    Player*         m_player;

    bool            m_BarrelIsEeaten;

public:
    matrix4x4f trafo;
    float           getDistanceToPlayer();

    bool isEaten() { return m_BarrelIsEeaten; };
    Barrel( Player*, vector<drawelement*>, float x, float y );
    ~Barrel();
    void draw();
    void draw(texture_ref, matrix4x4f*);
};

class BarrelContainer {
private:

    vector<Barrel*> m_Barrels;
    SplineMesh*     m_spline;
    int             m_nBarrels;

    Player*         m_player;
    float           m_playermindist;

    vector<drawelement*> mesh;

public:
    BarrelContainer( Player*, SplineMesh*, int );
    ~BarrelContainer();
    void setBack(SplineMesh* s, int Barrels);
    void draw();
    void draw(texture_ref, matrix4x4f*);
};

#endif // BARREL_H
