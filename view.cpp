#include "view.h"

View::View(vec3f& pos, vec3f& dir, vec3f& up, float fovy, float aspect, float near, float far) {

	cam =  make_perspective_cam("cam", &pos, &dir, &up, fovy, aspect, near, far);
	use_camera(cam);

}

View::~View() {

}

void View::moveCamera(vec3f& v) {

	vec3f oldPos, newPos, dir, up;
	extract_pos_vec3f_of_matrix(&oldPos, lookat_matrix_of_cam(cam));
	extract_dir_vec3f_of_matrix(&dir, lookat_matrix_of_cam(cam));
	extract_up_vec3f_of_matrix(&up, lookat_matrix_of_cam(cam));
	add_components_vec3f(&newPos, &oldPos, &v);
	change_lookat_of_cam(cam, &newPos, &dir, &up);
	recompute_gl_matrices_of_cam(cam);

}

void View::moveCameraToPos(vec3f& pos) {

	vec3f dir, up;
	extract_dir_vec3f_of_matrix(&dir, lookat_matrix_of_cam(cam));
	extract_up_vec3f_of_matrix(&up, lookat_matrix_of_cam(cam));
	change_lookat_of_cam(cam, &pos, &dir, &up);
	recompute_gl_matrices_of_cam(cam);

}