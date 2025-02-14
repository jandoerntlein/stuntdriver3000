#ifndef SPLINE_H
#define SPLINE_H

#include <libcgl/camera.h>
#include <map>
#include <vector>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <libcgl/mesh.h>
#include "globalConstants.h"

using namespace std;

class Spline {
private:
	vector<vec3f> control_point_vector;
	vector<vec3f> tanget_vector;
public:
	Spline();
	Spline(Spline s, vec3f v);
	~Spline();
	vector<vec3f> getControlPoints();
	int getNumControlPoints();
	vector<vec3f> generateControlPoints(int num, float from, float to);
	void refreshSpline();
	float getY(float x);
	bool isInsideSpline(float x);
	vec3f calculatePointOnSpline(int before, int after, float t);
	vec3f getTangetAtControlPoint(int i);
	vec3f getTangetAtX(float x);
	vec3f getNormalAt(float x);
	vector<vec3f> createLineStrip(int numberOfLines);
	void moveSpline(vec3f v);
};

#endif // SPLINE_H
