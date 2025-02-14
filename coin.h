#ifndef COIN_H
#define COIN_H

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

class Coin {
private:

    // graphic stuff
    vector<drawelement*> coin;
    float           m_rotationSpeed;

    void            update();
    Player*         m_player;

    bool            m_coinIsEeaten;
public:
    matrix4x4f trafo;

    float           getDistanceToPlayer();
    bool isEaten() { return m_coinIsEeaten; };
    Coin( Player*, vector<drawelement*>, float x, float y );
    ~Coin();
    void draw();
    void draw(texture_ref, matrix4x4f*);
};

class CoinContainer {
private:

    vector<Coin*>   m_coins;
    SplineMesh*     m_spline;
    int             m_nCoins;

    Player*         m_player;
    float           m_playermindist;

    vector<drawelement*> mesh;

public:
    CoinContainer( Player*, SplineMesh*, int );
    ~CoinContainer();
    void setBack(SplineMesh* s, int coins);
    void draw();
    void draw(texture_ref, matrix4x4f*);
};


#endif // COIN_H
