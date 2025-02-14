#ifndef VIEW_H
#define VIEW_H

#include <libcgl/camera.h>

using namespace std;

class View {
private:
	
	
public:
	View(vec3f& pos, vec3f& dir, vec3f& up, float fovy, float aspect, float near, float far);
	~View();
	void moveCamera(vec3f& v);
	void moveCameraToPos(vec3f& pos);
	camera_ref cam;
};

#endif // VIEW_H
