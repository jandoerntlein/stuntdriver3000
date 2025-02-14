#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <libccgl-0.0.1/cgl-adaptor.h>
#include <libccgl-0.0.1/objloader-addon.h>
#include <chipmunk/chipmunk.h>
#include <iostream>
#include "splinemesh.h"
#include "light.h"
#include "globalConstants.h"
#include "popup.h"
#include "text2d.h"
using namespace std;
using namespace ccgl;

extern Text2D*     text2d;

class Player {
private:
	
	#define CHASSIS 2
	#define WHEEL 1

	SplineMesh *splineMesh;

	// player is a car
	cpBody*		m_body;		// rigid body
	cpShape*    frontWheel;	// collision shapes
	cpShape* rearWheel;
	cpShape* chassis;
	cpSpace* space;

	cpFloat		m_moment;

	Popup*		popup_bam;
	Popup*		popup_bang;
	Popup*		popup_pow;
	Popup*		popup_zap;
	int			popup_decision;

	// graphic stuff
	vector<drawelement*> buggy;
	
	bool   outOfGas;
	int    loopings;
	bool   grounded;
	bool   flying;
	float  loopAngle;
	bool   clockwise;
	bool   loopFlag;
	bool   explosionFlag;
	bool   barrelFlag;
	int    coins;

	int overdrive;
	int overdrives;
	bool overdriveFlag;
	bool pulldownFlag;
	bool pulldownFlagRst;

	int landingCounter;
	int landingGrade;

public:
	matrix4x4f trafo;

	inline float getX() { return trafo.row_col( 0, 3 ); };
	inline float getY() { return trafo.row_col( 1, 3 ); };

	inline void addCoin() { coins++; return; };
	inline void addBarrel() { barrelFlag = true; return; };
	inline int getCoins() { return coins; };
	inline int getOverdrives() { return overdrives; };

	Player( cpSpace*, SplineMesh *splineMesh );
	~Player();
	void setBack(cpSpace* space);
	void makePhysics(cpSpace* space);
	void update();

	void draw( texture_ref, matrix4x4f* );
	void draw();
	
	float getRotation() { return cpBodyGetAngle( m_body ); };
	void rotateLeft();
	void rotateRight();
	void setOutOfGas();
	bool isOutOfGas();
	float getDistanceToGround();
	void pullDown();
	static void chassisToSpline(cpBody *body, cpArbiter *arb, bool *touch);
	static void wheelToSpline(cpBody *body, cpArbiter *arb, bool *touch);
	bool chassisTouchesGround();
	bool wheelTouchesGround();
	bool loopFlagSet();
	bool explosionFlagSet();
	bool barrelFlagSet();
	inline bool overdriveFlagSet() { return overdriveFlag; };
	int getLoopings();
	float getDistanceTo(float x, float y);
	int getLandingGrade();
	
};

#endif // PLAYER_H
