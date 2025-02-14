#ifndef GAME_H
#define GAME_H

#include <iostream>
#include "globalConstants.h"
#include "player.h"
#include "text2d.h"
#include "coin.h"
#include "barrel.h"
#include "particles.h"
#include <chipmunk/chipmunk.h>
#include "timer.h"
#include "shadow.h"
#include "hud.h"
#include "background.h"
#include "wood.h"

extern Text2D*     text2d;

using namespace std;
using namespace ccgl;

class Game {
private:
	
	// chipmunk stuff 
	cpSpace*    g_Space;                                   // global physics space
	float deltaTime;
	
	float gasoline;
	float distance;
	int coins;
	int loopings;
	int overdrives;

	float oldX;
	int frame;

	int highscore;
	int highscorePoints;

	int distTime;
	int distTimeDist;
	int distTimeTime;

	ostringstream distTimeOut;

	Timer* countdown;
	Timer* score;

	// fps stuff
	Timer *timer;
	int frames;
	float fps;

	framebuffer_ref m_fbo;
	texture_ref     m_depth;
	texture_ref     m_splitLeft;
	texture_ref     m_splitRight;

	void initFBO();

public:

	Hud*		   hud;
	SplineMesh*    splinemesh;
	Player*        player;
	CoinContainer* coin;
	Particles*     particles;
	CarStripes*    carstripes;
	Fire*		   overdrive;
	BarrelContainer* barrel;
	Shadow*			player_shadow;
	Background* background;
	WoodContainer*	 wood;

	Game();
	~Game();
	inline float getFPS() { return fps; };
	void newGame( bool newSpline = false );
	void addGas(float amount);
	void subGas();
	void render();
	void update();
	void showGas();
	int getScore();
	CoinContainer* getCoins();
	int calculateHighscorePoints();
	void showScore();
	void showDistAndPoints();
	
};

#endif // GAME_H
