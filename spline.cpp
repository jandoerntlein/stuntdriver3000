#include "spline.h"




/*
*	
*/
Spline::Spline() {

	srand(time(NULL));
	refreshSpline();

}

Spline::Spline(Spline s, vec3f v) {

	vec3f tmp;
	for (int i = 0; i < constant::NUM_CONTROL_POINTS; i++) {
		tmp = {s.getControlPoints()[i].x + v.x, s.getControlPoints()[i].y + v.y, s.getControlPoints()[i].z + v.z};
		control_point_vector.push_back(tmp);
	}
	for (int i = 0; i < constant::NUM_CONTROL_POINTS; i++) {
		tanget_vector.push_back(getTangetAtControlPoint(i));
	}

}

Spline::~Spline() {

}

vector<vec3f> Spline::getControlPoints() {
	return control_point_vector;
}

int Spline::getNumControlPoints() {
	return constant::NUM_CONTROL_POINTS;
}

vector<vec3f> Spline::generateControlPoints(int num, float from, float to) {

	float x,y,ran;
	float lenX = (to - from) / (num - 1);
	vector<vec3f> vec;
	vec3f v;
	int dif = (int)((constant::MAX_UP - constant::MAX_DOWN) * 1000);

	x = from - constant::START_STRIP_LENGTH;
	y = 0;
	v = {x, y, 0};
	vec.push_back(v);
	x = from - constant::START_STRIP_LENGTH * 0.4;
	y = 0;
	v = {x, y, 0};
	vec.push_back(v);
	x = from - constant::START_STRIP_LENGTH * 0.2;
	y = 0;
	v = {x, y, 0};
	vec.push_back(v);

	for (int i = 0; i < num; i++) {
		x = from + i*lenX;
		ran = (float)(rand() % dif) / 1000 + constant::MAX_DOWN;
		y += ran;
		v = {x, y, 0};
		vec.push_back(v);
	}

	return vec;

}

void Spline::refreshSpline() {

	control_point_vector.clear();
	tanget_vector.clear();

	control_point_vector = generateControlPoints(constant::NUM_CONTROL_POINTS - 3, constant::START_COORD, constant::END_COORD);

	for (int i = 0; i < constant::NUM_CONTROL_POINTS; i++) {
		tanget_vector.push_back(getTangetAtControlPoint(i));
	}

}

float Spline::getY(float x) {

	if (isInsideSpline(x)) {
		
		int i = 0;
		while(x >= control_point_vector[i].x) {
			i++;
		}

		float t = (x - control_point_vector[i-1].x) / (control_point_vector[i].x - control_point_vector[i-1].x);

		return calculatePointOnSpline(i-1, i, t).y;

	}

	return 0;

}

bool Spline::isInsideSpline(float x) {

	if (x < control_point_vector[0].x || x > control_point_vector[constant::NUM_CONTROL_POINTS - 1].x) return false;
	return true;

}

vec3f Spline::calculatePointOnSpline(int before, int after, float t) {

	vec3f v = {(1 - 3*pow(t,2) + 2*pow(t,3)) * control_point_vector[before].x
			+ (3*pow(t,2) - 2*pow(t,3)) * control_point_vector[after].x
			+ (t - 2*pow(t,2) + pow(t,3)) * tanget_vector[before].x
			+ (-pow(t,2) + pow(t,3)) * tanget_vector[after].x,
			(1 - 3*pow(t,2) + 2*pow(t,3)) * control_point_vector[before].y
			+ (3*pow(t,2) - 2*pow(t,3)) * control_point_vector[after].y
			+ (t - 2*pow(t,2) + pow(t,3)) * tanget_vector[before].y
			+ (-pow(t,2) + pow(t,3)) * tanget_vector[after].y,
			control_point_vector[before].z};
	return v;

}

vec3f Spline::getTangetAtControlPoint(int i) {

	if (i == 0 || i == constant::NUM_CONTROL_POINTS - 1) {
		vec3f v = {1, 0, 0};
		return v;
	} else {
		vec3f before = control_point_vector[i - 1];
		vec3f after = control_point_vector[i + 1];
		vec3f tangent = {(after.x - before.x) / 2, (after.y - before.y) / 2, 0};
		return tangent;
	}

}

vec3f Spline::getTangetAtX(float x) {

	if (x < constant::START_COORD - 50.0 || x > constant::END_COORD) {
		vec3f v = {1, 0, 0};
		return v;
	} else {
		float y1 = getY(x - 0.001);
		float y2 = getY(x + 0.001);
		vec3f tangent = {0.002, y2 - y1, 0};
		return tangent;
	}

}

vec3f Spline::getNormalAt(float x) {

	vec3f tangent = getTangetAtX(x);
	vec3f normal = {-tangent.y, tangent.x, 0};
	normalize_vec3f( &normal );
	return normal;

}

vector<vec3f> Spline::createLineStrip(int numberOfLines) {

	vector<vec3f> vec;
	if (numberOfLines <= 0) return vec;
	int i = 0;
	float lenX = (control_point_vector[constant::NUM_CONTROL_POINTS - 1].x - control_point_vector[0].x) / numberOfLines;
	
	while (i < numberOfLines) {
		vec3f v = {control_point_vector[0].x + i * lenX, getY(control_point_vector[0].x + i * lenX), control_point_vector[0].z};
		vec.push_back(v);
		i++;
	}
	vec.push_back(control_point_vector[constant::NUM_CONTROL_POINTS - 1]);
	
	return vec;

}

void Spline::moveSpline(vec3f v) {

	for (int i = 0; i < constant::NUM_CONTROL_POINTS; i++) {
		control_point_vector[i] = {control_point_vector[i].x + v.x, control_point_vector[i].y + v.y, control_point_vector[i].z + v.z};
	}

}