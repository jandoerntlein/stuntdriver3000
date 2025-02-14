#ifndef LIGHT_H
#define LIGHT_H

static void setup_light( ccgl::shader_ptr shader, 
				  vec3f *dir,	// shines in which direction?
				  vec3f *col,	// light color
				  float intensity ) {

	normalize_vec3f( dir );
	normalize_vec3f( col );

	int loc = glGetUniformLocation(gl_shader_object(shader->cgl_ref()), "light_dir");
	glUniform3f(loc, dir->x, dir->y, dir->z);

	loc = glGetUniformLocation(gl_shader_object(shader->cgl_ref()), "light_col");
	glUniform3f(loc, col->x, col->y, col->z);

	loc = glGetUniformLocation(gl_shader_object(shader->cgl_ref()), "light_intensity");
	glUniform1f(loc, intensity);
}

#endif // LIGHT_H